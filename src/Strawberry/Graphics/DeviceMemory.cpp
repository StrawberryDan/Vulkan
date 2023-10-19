//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "DeviceMemory.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	DeviceMemory::DeviceMemory(const Device& device, uint32_t size, VkMemoryPropertyFlags flags)
		: mDevice(device.mDevice)
	{
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(device.mPhysicalDevice, &memoryProperties);

		Core::Optional<uint32_t> memoryTypeIndex;
		for (int i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if (flags & (1 << i))
			{
				memoryTypeIndex = i;
				break;
			}
		}


		VkMemoryAllocateInfo allocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = size,
			.memoryTypeIndex = memoryTypeIndex.Unwrap(),
		};

		Core::AssertEQ(vkAllocateMemory(mDevice, &allocateInfo, nullptr, &mDeviceMemory), VK_SUCCESS);
	}


	DeviceMemory::DeviceMemory(DeviceMemory&& rhs)
		: mDeviceMemory(std::exchange(rhs.mDeviceMemory, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
	{}


	DeviceMemory& DeviceMemory::operator=(DeviceMemory&& rhs)
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	DeviceMemory::~DeviceMemory()
	{
		if (mDevice)
		{
			vkFreeMemory(mDevice, mDeviceMemory, nullptr);
		}
	}
}
