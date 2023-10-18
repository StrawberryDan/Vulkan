//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Device.hpp"
// Strawberry Graphics
#include "Instance.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <utility>
#include <vector>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Device::Device(const Instance& instance)
		: mDevice{}
		, mInstance(instance.mInstance)
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance.mInstance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance.mInstance, &physicalDeviceCount, physicalDevices.data());

		std::vector<VkPhysicalDeviceProperties> properties;
		std::transform(physicalDevices.begin(), physicalDevices.end(), std::back_inserter(properties),
					   [](auto x)
					   {
						   VkPhysicalDeviceProperties properties;
						   vkGetPhysicalDeviceProperties(x, &properties);
						   return properties;
					   });

		mPhysicalDevice = physicalDevices[0];

		std::vector<VkDeviceQueueCreateInfo> queues;
		std::vector<const char*> layers;
		std::vector<const char*> extensions
			{
				"VK_KHR_swapchain"
			};
		VkPhysicalDeviceFeatures features{};



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

		Core::AssertEQ(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice), VK_SUCCESS);
	}


	Device::Device(Device&& rhs)
		: mPhysicalDevice(std::exchange(rhs.mPhysicalDevice, nullptr))
		  , mDevice(std::exchange(rhs.mDevice, nullptr)) {}


	Device& Device::operator=(Device&& rhs)
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
		if (mDevice)
		{
			vkDestroyDevice(mDevice, nullptr);
		}
	}
}
