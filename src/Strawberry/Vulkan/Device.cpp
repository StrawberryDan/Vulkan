//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
#include "Device.hpp"
#include "Instance.hpp"
#include "Memory/Allocator.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <algorithm>
#include <utility>
#include <vector>
#include <numeric>
#include <cstring>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Device::Device(const PhysicalDevice& physicalDevice, std::vector<QueueCreateInfo> queueCreateInfo)
		: mDevice{}
		, mPhysicalDevice(physicalDevice)
	{
		// Describes Queues
		std::vector<VkDeviceQueueCreateInfo> queues;
		std::vector<std::vector<float>>      queuePriorities;
		for (auto& info: queueCreateInfo)
		{
			queuePriorities.emplace_back(info.count, 1.0f);
			queues.push_back(VkDeviceQueueCreateInfo{
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
			"VK_KHR_swapchain"
		};
		// Select Features
		VkPhysicalDeviceFeatures features{};


		// Enumerate Extension Properties of Physical Device
		auto extensionProperties = GetPhysicalDevice().GetExtensionProperties();


		// Add portability subset if available
		if (std::any_of(extensionProperties.begin(),
		                extensionProperties.end(),
		                [](VkExtensionProperties x)
		                {
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


		for (auto& createInfo: queueCreateInfo)
		{
			for (int i = 0; i < createInfo.count; i++)
				mQueues[createInfo.familyIndex].emplace_back(Queue(*this, createInfo.familyIndex, i));
		}
	}


	Device::Device(Device&& rhs) noexcept
		: mDevice(std::exchange(rhs.mDevice, nullptr))
		, mPhysicalDevice(std::move(rhs.mPhysicalDevice))
		, mQueues(std::move(rhs.mQueues)) {}


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
		mQueues.clear();
		if (mDevice)
		{
			Core::Assert(vkDeviceWaitIdle(mDevice) == VK_SUCCESS);
			vkDestroyDevice(mDevice, nullptr);
		}
	}


	Device::operator VkDevice() const
	{
		return mDevice;
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
}
