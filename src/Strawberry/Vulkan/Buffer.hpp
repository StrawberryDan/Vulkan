#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Standard Library
#include <concepts>
#include "DeviceMemory.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;


	class Buffer
	{
	public:
		Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage);
		Buffer(const Device& device, const Core::IO::DynamicByteBuffer& bytes, VkBufferUsageFlags usage);
		Buffer(const Buffer& rhs) = delete;
		Buffer& operator=(const Buffer& rhs) = delete;
		Buffer(Buffer&& rhs) noexcept;
		Buffer& operator=(Buffer&& rhs) noexcept;
		~Buffer();


		operator VkBuffer() const;


		VkDevice GetDevice() const;


		template<std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


		void SetData(const Core::IO::DynamicByteBuffer& bytes);


		[[nodiscard]] uint64_t GetSize() const;
		Core::IO::DynamicByteBuffer GetBytes() const;


	private:
		uint64_t mSize;
		VkBuffer mBuffer;
		DeviceMemory mMemory;
		VkDevice mDevice;

		Core::IO::DynamicByteBuffer mBytes;
	};
}
