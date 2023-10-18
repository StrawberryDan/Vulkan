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


	class Buffer
	{
	public:
		Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage);
		Buffer(const Buffer& rhs) = delete;
		Buffer& operator=(const Buffer& rhs) = delete;
		Buffer(Buffer&& rhs) noexcept ;
		Buffer& operator=(Buffer&& rhs) noexcept ;
		~Buffer();


	private:
		VkBuffer mBuffer;
		VkDevice mDevice;
	};
}
