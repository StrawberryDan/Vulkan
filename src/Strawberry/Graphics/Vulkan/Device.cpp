//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Device.hpp"
// Strawberry Graphics
#include "Instance.hpp"
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
namespace Strawberry::Graphics::Vulkan
{
	static Core::Optional<uint32_t> SelectQueueFamily(const std::vector<VkQueueFamilyProperties>& properties)
	{
		std::vector<uint32_t> candidates(properties.size(), 0);
		std::iota(candidates.begin(), candidates.end(), 0);
		std::erase_if(candidates, [&](uint32_t x)
		{
			const bool graphics = properties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT;

			return !graphics;
		});

		auto score = [](uint32_t x) -> uint32_t
		{
			return 0;
		};

		std::sort(candidates.begin(), candidates.end(), [&](uint32_t a, uint32_t b)
		{ return score(a) > score(b); });

		return candidates.empty() ? Core::NullOpt : Core::Optional(candidates[0]);
	}


	static Core::Optional<std::tuple<VkPhysicalDevice, uint32_t, uint32_t>>
	SelectPhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevices)
	{
		// Get Device Properties
		std::vector<VkPhysicalDeviceProperties> properties;
		std::transform(physicalDevices.begin(), physicalDevices.end(), std::back_inserter(properties),
			[](auto x)
			{
			   VkPhysicalDeviceProperties properties;
			   vkGetPhysicalDeviceProperties(x, &properties);
			   return properties;
			});

		// Get Queue Family Properties Function
		auto getQueueFamilyProperties = [](VkPhysicalDevice device)
		{
			uint32_t count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
			std::vector<VkQueueFamilyProperties> familyProperties(count);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, familyProperties.data());
			return familyProperties;
		};

		// Get the queue family properties of each physical device.
		std::vector<std::vector<VkQueueFamilyProperties>> queueFamilyProperties;
		std::transform(physicalDevices.begin(), physicalDevices.end(), std::back_inserter(queueFamilyProperties),
					   getQueueFamilyProperties);

		// Create a set of candidates, indexes into the physicalDevices list
		std::vector<uint32_t> candidates(physicalDevices.size(), 0);
		std::iota(candidates.begin(), candidates.end(), 0);


		// Get an applicable queue family from each device
		std::vector<Core::Optional<uint32_t>> selectedQueueFamily;
		std::transform(candidates.begin(), candidates.end(), std::back_inserter(selectedQueueFamily),
			[&](uint32_t x) { return SelectQueueFamily(queueFamilyProperties[x]); });
		// Erase candidates with no eligible queue families
		std::erase_if(candidates, [&](uint32_t x) { return !selectedQueueFamily[x].HasValue(); });
		if (candidates.empty()) return {};

		// Device Scoring Function
		auto score = [&](uint32_t x)
		{
			uint32_t score = 0;
			if (properties[x].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
			return score;
		};

		// Device sorting function. Best device comes first
		auto sort = [&](uint32_t a, uint32_t b)
		{
			return score(a) > score(b);
		};
		// Sort Devices by score
		std::sort(candidates.begin(), candidates.end(), sort);

		return std::make_tuple(physicalDevices[candidates[0]], selectedQueueFamily[candidates[0]].Value(), uint32_t(1));
	}


	Device::Device(const Instance& instance)
			: mDevice{}
			, mInstance(instance.mInstance)
	{
		// Enumerate Physical Devices
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance.mInstance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance.mInstance, &physicalDeviceCount, physicalDevices.data());

		// Select Best Device
		auto [physicalDevice, queueFamilyIndex, queueCount] = SelectPhysicalDevice(physicalDevices).Unwrap();
		mPhysicalDevice = physicalDevice;
		mQueueFamilyIndex = queueFamilyIndex;

		// Select Queue Family
		std::vector<float> priorities(queueCount, 1.0);

		// Describes Queues
		std::vector<VkDeviceQueueCreateInfo> queues;
		queues.push_back(VkDeviceQueueCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = queueFamilyIndex,
				.queueCount = queueCount,
				.pQueuePriorities = priorities.data()
		});

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
		uint32_t extensionPropertyCount = 0;
		vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionPropertyCount, nullptr);
		std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
		vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionPropertyCount,
											 extensionProperties.data());


		// Add portability subset if available
		if (std::any_of(extensionProperties.begin(), extensionProperties.end(), [](VkExtensionProperties x)
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
		Core::AssertEQ(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice), VK_SUCCESS);
	}


	Device::Device(Device&& rhs) noexcept
			: mPhysicalDevice(std::exchange(rhs.mPhysicalDevice, nullptr)),
			  mDevice(std::exchange(rhs.mDevice, nullptr)),
			  mInstance(std::exchange(rhs.mInstance, nullptr))
	{}


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
		if (mDevice)
		{
			vkDestroyDevice(mDevice, nullptr);
		}
	}


	VkPhysicalDevice Device::GetPhysicalDevice() const
	{
		return mPhysicalDevice;
	}


	VkPhysicalDeviceLimits Device::GetPhysicalDeviceLimits() const
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &properties);
		return properties.limits;
	}
}
