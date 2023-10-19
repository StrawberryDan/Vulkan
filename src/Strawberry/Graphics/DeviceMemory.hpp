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


	public:
		DeviceMemory() = default;
		DeviceMemory(const Device& device, uint32_t size, VkMemoryPropertyFlags flags);
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
