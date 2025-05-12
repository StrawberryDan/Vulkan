#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/Allocator.hpp"
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;


	class Buffer
	{
	public:
		// Allocate a buffer with the given size and usage from the given allocator.
		Buffer(Allocator& allocator,
			size_t size,
			VkBufferUsageFlags usage);
		// Allocate a buffer from the given allocator, and copy the data given, with the set usage flags.
		Buffer(Allocator& allocator,
		       const Core::IO::DynamicByteBuffer& bytes,
		       VkBufferUsageFlags                 usage);

		Buffer(const Buffer& rhs)            = delete;
		Buffer& operator=(const Buffer& rhs) = delete;
		Buffer(Buffer&& rhs) noexcept;
		Buffer& operator=(Buffer&& rhs) noexcept;
		~Buffer();


		// Implicit conversion to the vulkan handle for this buffer.
		operator VkBuffer() const;


		// Return the vulkan handle of this buffer's device.
		VkDevice GetDevice() const;


		// Overwrite the region of memory mapped to this buffer
		void SetData(const Core::IO::DynamicByteBuffer& bytes);

		// Returns the address to the mapped memory location for this buffer;
		[[nodiscard]]       uint8_t* GetData();
		// Returns the address to the mapped memory location for this buffer;
		[[nodiscard]] const uint8_t* GetData() const;

		// Returns the size of this buffer.
		[[nodiscard]] uint64_t GetSize() const;

	private:
		// Pointer to the allocator that allocated us.
		Core::ReflexivePointer<Allocator> mAllocator;
		// The amount of space allocated to this buffer.
		uint64_t                          mSize;
		// The handle for this buffer.
		VkBuffer                          mBuffer;
		// The memory allocated to this buffer.
		Allocation                        mMemory;
	};
}
