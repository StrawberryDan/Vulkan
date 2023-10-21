#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;


	class DeviceMemory
	{
		friend class Buffer;
		friend class Image;


	public:
		DeviceMemory() = default;
		DeviceMemory(const Device& device, uint32_t size, uint32_t flags, VkMemoryPropertyFlags properties);
		DeviceMemory(const DeviceMemory& rhs) = delete;
		DeviceMemory& operator=(const DeviceMemory& rhs) = delete;
		DeviceMemory(DeviceMemory&& rhs);
		DeviceMemory& operator=(DeviceMemory&& rhs);
		~DeviceMemory();


	private:
		VkDeviceMemory mDeviceMemory = nullptr;
		VkDevice mDevice = nullptr;
	};
}
