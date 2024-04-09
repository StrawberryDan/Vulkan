//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "PhysicalDevice.hpp"
#include "Instance.hpp"
// Standard Library
#include <algorithm>


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	PhysicalDevice::PhysicalDevice(const Instance& instance, VkPhysicalDevice rawHandle)
		: mPhysicalDevice(rawHandle)
		, mInstance(instance)
	{}


	PhysicalDevice::PhysicalDevice(PhysicalDevice&& other)
		: mPhysicalDevice(std::exchange(other.mPhysicalDevice, VK_NULL_HANDLE))
		, mInstance(std::move(other.mInstance))
		, mFeatures(std::move(other.mFeatures))
		, mProperties(std::move(other.mProperties))
		, mQueueFamilyProperties(std::move(other.mQueueFamilyProperties))
	{}


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


	const VkPhysicalDeviceMemoryProperties PhysicalDevice::GetMemoryProperties() const
	{
		if (!mMemoryProperties)
		{
			mMemoryProperties.Emplace();
			vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties.Value());
		}

		return mMemoryProperties.Value();
	}


	const std::vector<VkExtensionProperties>& PhysicalDevice::GetExtensionProperties() const
	{
		if (!mExtensionProperties)
		{
			uint32_t extensionPropertyCount = 0;
			vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionPropertyCount, nullptr);

			mExtensionProperties.Emplace(extensionPropertyCount);
			std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
			vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionPropertyCount,
												 mExtensionProperties->data());
		}

		return mExtensionProperties.Value();
	}


	std::vector<uint32_t> PhysicalDevice::SearchQueueFamilies(std::underlying_type_t<VkQueueFlagBits> flagBits) const
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

		std::sort(familyIndices.begin(), familyIndices.end(), [this](uint32_t a, uint32_t b)
		{
			return GetQueueFamilyProperties()[a].queueCount > GetQueueFamilyProperties()[b].queueCount;
		});

		return familyIndices;
	}


	std::vector<uint32_t> PhysicalDevice::SearchMemoryTypes(uint32_t typeBits, VkMemoryPropertyFlags properties) const
	{
		std::vector<uint32_t> memoryTypes;
		auto memoryProperties = GetMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			const bool validType = typeBits & (1 << i);
			const bool propertiesAvailable = (properties & memoryProperties.memoryTypes[i].propertyFlags) == properties;
			if (validType && propertiesAvailable)
			{
				memoryTypes.emplace_back(i);
			}
		}

		return memoryTypes;
	}
}
