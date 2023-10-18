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
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance.mInstance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance.mInstance, &physicalDeviceCount, physicalDevices.data());

		mPhysicalDevice = physicalDevices[0];

		std::vector<VkDeviceQueueCreateInfo> queues;
		std::vector<const char*> layers;
		std::vector<const char*> extensions;
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
}
