//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Device/PhysicalDevice.hpp"
#include "Strawberry/Vulkan/Device/Instance.hpp"
// Standard Library
#include <algorithm>
#include <numeric>
#include <ranges>


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	PhysicalDevice::PhysicalDevice(Instance& instance, VkPhysicalDevice rawHandle)
		: mPhysicalDevice(rawHandle)
		, mInstance(instance) {}


	PhysicalDevice::PhysicalDevice(PhysicalDevice&& other)
		: mPhysicalDevice(std::exchange(other.mPhysicalDevice, VK_NULL_HANDLE))
		, mInstance(std::move(other.mInstance))
		, mFeatures(std::move(other.mFeatures))
		, mProperties(std::move(other.mProperties))
		, mQueueFamilyProperties(std::move(other.mQueueFamilyProperties)) {}


	PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other)
	{
		if (this != &other)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}


	Core::ReflexivePointer<Instance> PhysicalDevice::GetInstance() const
	{
		return mInstance;
	}


	VkPhysicalDeviceType PhysicalDevice::GetType() const
	{
		return GetProperties().deviceType;
	}


	const VkPhysicalDeviceProperties& PhysicalDevice::GetProperties() const
	{
		if (!mProperties)
		{
			mProperties.Emplace();
			vkGetPhysicalDeviceProperties(mPhysicalDevice, &mProperties.Value());
		}

		return mProperties.Value();
	}


	const VkPhysicalDeviceLimits& PhysicalDevice::GetLimits() const
	{
		return GetProperties().limits;
	}


	const VkPhysicalDeviceFeatures& PhysicalDevice::GetFeatures() const
	{
		if (!mFeatures)
		{
			mFeatures.Emplace();
			vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mFeatures.Value());
		}

		return mFeatures.Value();
	}


	const std::vector<VkQueueFamilyProperties>& PhysicalDevice::GetQueueFamilyProperties() const
	{
		if (!mQueueFamilyProperties)
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
			mQueueFamilyProperties.Emplace(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, mQueueFamilyProperties->data());
		}

		return mQueueFamilyProperties.Value();
	}


	const VkPhysicalDeviceMemoryProperties& PhysicalDevice::GetMemoryProperties() const
	{
		if (!mMemoryProperties)
		{
			mMemoryProperties.Emplace();
			vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties.Value());
		}

		return mMemoryProperties.Ref();
	}


	const std::vector<VkExtensionProperties>& PhysicalDevice::GetExtensionProperties() const
	{
		if (!mExtensionProperties)
		{
			uint32_t extensionPropertyCount = 0;
			vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionPropertyCount, nullptr);

			mExtensionProperties.Emplace(extensionPropertyCount);
			std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
			Core::AssertEQ(
				vkEnumerateDeviceExtensionProperties(
					mPhysicalDevice,
					nullptr,
					&extensionPropertyCount,
					mExtensionProperties->data()),
				VK_SUCCESS);
		}

		return mExtensionProperties.Value();
	}


	std::vector<uint32_t> PhysicalDevice::SearchQueueFamilies(VkQueueFlags flagBits) const
	{
		std::vector<uint32_t> familyIndices;

		auto& queueFamilies = GetQueueFamilyProperties();
		for (int i = 0; i < queueFamilies.size(); i++)
		{
			auto& queueFamily = queueFamilies[i];
			if ((queueFamily.queueFlags & flagBits) == flagBits)
			{
				familyIndices.emplace_back(i);
			}
		}

		std::sort(familyIndices.begin(),
		          familyIndices.end(),
		          [this](uint32_t a, uint32_t b)
		          {
			          return GetQueueFamilyProperties()[a].queueCount > GetQueueFamilyProperties()[b].queueCount;
		          });

		return familyIndices;
	}


	std::vector<MemoryType> PhysicalDevice::SearchMemoryTypes(const MemoryTypeCriteria& memoryCriteria) const
	{
		VkMemoryPropertyFlags requiredProperties                   = memoryCriteria.requiredProperties;
		VkMemoryPropertyFlags preferredProperties                  = memoryCriteria.preferredProperties;
		const auto&           [typeCount, types, heapCount, heaps] = GetMemoryProperties();
		std::vector<uint32_t> memoryTypes(typeCount);
		std::iota(memoryTypes.begin(), memoryTypes.end(), 0);

		std::vector<uint32_t> viableMemoryTypes;
		// Guess that half of the memory types are available.
		// This is probably a sensible assumption.
		viableMemoryTypes.reserve(typeCount / 2);
		for (uint32_t type: memoryTypes)
		{
			const bool hasRequiredProperties = requiredProperties == (types[type].propertyFlags & requiredProperties);
			const bool isLargeEnough = mMemoryProperties->memoryHeaps[types[type].heapIndex].size >= memoryCriteria.minimumSize;
			if (hasRequiredProperties && isLargeEnough)
			{
				viableMemoryTypes.emplace_back(type);
			}
		}


		std::vector<uint32_t> preferredMemoryTypes;
		preferredMemoryTypes.reserve(typeCount);
		for (uint32_t type: viableMemoryTypes)
		{
			if (preferredProperties == (types[type].propertyFlags & preferredProperties))
			{
				preferredMemoryTypes.emplace_back(type);
			}
		}


		std::vector<uint32_t> candidates = !preferredMemoryTypes.empty()
		                                   ? preferredMemoryTypes
		                                   : !viableMemoryTypes.empty()
		                                     ? viableMemoryTypes
		                                     : std::vector<uint32_t>();


		return candidates
				| std::ranges::views::transform([&](uint32_t type)
				{
					return MemoryType{
						.index = MemoryTypeIndex{ .physicalDevice = mPhysicalDevice, .memoryTypeIndex = type },
						.heapSize = heaps[types[type].heapIndex].size,
						.properties = types[type].propertyFlags,
					};
				})
				| std::ranges::to<std::vector>();
	}
}
