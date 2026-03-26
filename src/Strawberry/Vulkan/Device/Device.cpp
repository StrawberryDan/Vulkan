//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
#include "Device.hpp"
#include "Strawberry/Vulkan/Device/Instance.hpp"
#include "Strawberry/Vulkan/Device/DescriptorPoolAllocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/FallbackAllocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/FreelistAllocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/NaivePolyAllocator.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <algorithm>
#include <deque>
#include <utility>
#include <vector>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	QueueCriteria QueueCriteria::Graphics()
	{
		return {.flags = VK_QUEUE_GRAPHICS_BIT };
	}

	QueueCriteria QueueCriteria::Transfer()
	{
		return {.flags = VK_QUEUE_TRANSFER_BIT };
	}

	QueueCriteria QueueCriteria::Compute()
	{
		return {.flags = VK_QUEUE_COMPUTE_BIT };
	}

	Device::Device(
		const PhysicalDevice&        physicalDevice,
		const VkPhysicalDeviceFeatures& features,
		std::vector<QueueCreateInfo> queueCreateInfo)
			: mDevice{}
			, mPhysicalDevice(physicalDevice)
	{
		ZoneScoped;

		// Describes Queues
		std::vector<VkDeviceQueueCreateInfo> queues;
		std::vector<std::vector<float> >     queuePriorities;
		for (auto& info: queueCreateInfo)
		{
			queuePriorities.emplace_back(info.count, 1.0f);
			queues.push_back
					(VkDeviceQueueCreateInfo
					 {
						 .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
						 .pNext = nullptr,
						 .flags = 0,
						 .queueFamilyIndex = info.familyIndex,
						 .queueCount = info.count,
						 .pQueuePriorities = queuePriorities.back().data()
					 });
		}

		// Select Layers
		std::vector<const char*> layers;

		// Select Extensions
		std::vector<const char*> extensions
		{
			"VK_KHR_dynamic_rendering",
			"VK_KHR_swapchain",
#ifdef STRAWBERRY_DEBUG
			"VK_KHR_shader_non_semantic_info",
#endif
		};

		// Enumerate Extension Properties of Physical Device
		auto extensionProperties = GetPhysicalDevice().GetExtensionProperties();


		// Add portability subset if available
		if (std::any_of(extensionProperties.begin(),
						extensionProperties.end(),
						[](VkExtensionProperties x) {
							return strcmp(x.extensionName, "VK_KHR_portability_subset") == 0;
						}))
		{
			extensions.push_back("VK_KHR_portability_subset");
		}


		// Populate info struct
		VkDeviceCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(queues.size()),
			.pQueueCreateInfos = queues.data(),
			.enabledLayerCount = static_cast<uint32_t>(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
			.pEnabledFeatures = &features
		};

		// Create Device
		Core::AssertEQ(vkCreateDevice(physicalDevice.mPhysicalDevice, &createInfo, nullptr, &mDevice), VK_SUCCESS);

		const auto& queueFamilyProperties = physicalDevice.GetQueueFamilyProperties();

		for (const auto& createInfo: queueCreateInfo)
		{
			for (int i = 0; i < createInfo.count; i++)
				mQueues[createInfo.familyIndex].emplace_back(Queue(
					*this,
					createInfo.familyIndex,
					i,
					queueFamilyProperties[createInfo.familyIndex].queueFlags));
		}

		mAllocator = std::make_unique<NaivePolyAllocator<FallbackChainAllocator<FreeListAllocator>>>(*this);
		mDescriptorPoolAllocator = std::make_unique<DescriptorPoolAllocator>(*this);
	}


	Device::Device(Device&& rhs) noexcept
		: mDevice(std::exchange(rhs.mDevice, nullptr))
		  , mPhysicalDevice(std::move(rhs.mPhysicalDevice))
		  , mQueues(std::move(rhs.mQueues))
		  , mAllocator(std::move(rhs.mAllocator))
		  , mDescriptorPoolAllocator(std::move(rhs.mDescriptorPoolAllocator)) {}


	Device& Device::operator=(Device&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Device::~Device()
	{
		ZoneScoped;

		if (mDevice)
		{
			WaitUntilIdle();
			mAllocator.reset();
			mQueues.clear();
			mAllocator.reset();
			mDescriptorPoolAllocator.reset();
			Core::Assert(vkDeviceWaitIdle(mDevice) == VK_SUCCESS);
			vkDestroyDevice(mDevice, nullptr);
		}
	}


	VkDevice Device::Handle() const
	{
		return mDevice;
	}


	Device::operator VkDevice() const
	{
		return mDevice;
	}


	void Device::WaitUntilIdle() const
	{
		ZoneScoped;

		Core::AssertEQ(vkDeviceWaitIdle(mDevice), VK_SUCCESS);
	}


	Core::ReflexivePointer<Instance> Device::GetInstance() const
	{
		return GetPhysicalDevice().GetInstance();
	}


	const PhysicalDevice& Device::GetPhysicalDevice() const
	{
		return *mPhysicalDevice;
	}


	Queue& Device::GetQueue(uint32_t family, uint32_t index)
	{
		return mQueues[family][index];
	}


	Queue& Device::GetQueue(const QueueCriteria& criteria, unsigned int order)
	{
		const auto& queueProperties = mPhysicalDevice->GetQueueFamilyProperties();

		for (auto& [index, family] : mQueues)
		{
			if ((queueProperties[index].queueFlags & criteria.flags) == criteria.flags)
			{
				return family[order % family.size()];
			}
		}

		Core::Unreachable();
	}


	PolyAllocator& Device::GetAllocator() const
	{
		return *mAllocator;
	}


	DescriptorSet Device::AllocateDescriptorSet(const DescriptorSetLayout& descriptorSetLayout)
	{
		return mDescriptorPoolAllocator->Allocate(*this, descriptorSetLayout);
	}

	Device::Builder::Builder(const PhysicalDevice& physicalDevice)
		: device(physicalDevice)
	{
		mFeatures = std::make_unique<VkPhysicalDeviceFeatures>();
		std::memset(mFeatures.get(), 0, sizeof(VkPhysicalDeviceFeatures));
	}


	Device::Builder& Device::Builder::WithQueue(const QueueCriteria& queueCriteria, unsigned int count)
	{
		const auto& queueProperties = device.GetQueueFamilyProperties();

		auto candidates = device.SearchQueueFamilies(queueCriteria) | std::ranges::to<std::deque>();

		std::erase_if(candidates, [&, this] (const uint32_t x) {
			return std::ranges::any_of(mQueueCreateInfo, [&] (const QueueCreateInfo& createInfo) {
				Core::Assert(createInfo.count <= queueProperties[x].queueCount);
				return createInfo.familyIndex == x && createInfo.count == queueProperties[x].queueCount;
			});
		});


		while (count > 0 && !candidates.empty())
		{
			uint32_t familyIndex = candidates.front();
			candidates.pop_front();

			auto search = std::ranges::find_if(mQueueCreateInfo,
				[&] (const QueueCreateInfo& queueCreateInfo) { return queueCreateInfo.familyIndex == familyIndex; });
			if (search != mQueueCreateInfo.end())
			{
				unsigned int max = queueProperties[familyIndex].queueCount - search->count;

				if (count <= max)
				{
					search->count += count;
					count = 0;
				}
				else
				{
					search->count = queueProperties[familyIndex].queueCount;
					count -= max;
				}
			}
			else
			{
				QueueCreateInfo createInfo;
				createInfo.familyIndex = familyIndex;
				unsigned int max = queueProperties[familyIndex].queueCount;

				if (count <= max)
				{
					createInfo.count = count;
					count = 0;
				}
				else
				{
					createInfo.count = queueProperties[familyIndex].queueCount;
					count -= max;
				}
				mQueueCreateInfo.emplace_back(createInfo);
			}
		}

		Core::AssertEQ(count, 0);

		return *this;
	}


	Device Device::Builder::Build()
	{
		return Device(device, *mFeatures, mQueueCreateInfo);
	}
}
