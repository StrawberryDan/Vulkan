#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include "DeviceMemory.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;


	class Buffer
	{
		friend class BufferView;


	public:
		Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage);
		Buffer(const Buffer& rhs) = delete;
		Buffer& operator=(const Buffer& rhs) = delete;
		Buffer(Buffer&& rhs) noexcept;
		Buffer& operator=(Buffer&& rhs) noexcept;
		~Buffer();


		template<std::movable T, typename... Args>
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		VkBuffer mBuffer;
		DeviceMemory mMemory;
		VkDevice mDevice;
	};
}
