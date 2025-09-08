#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/Allocator/SingleAllocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/MultiAllocator.hpp"
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;


	class Buffer
	{
	public:
		class Builder {
		public:
			Builder(Device& device, MemoryTypeCriteria memoryTypeCriteria);
			Builder(Allocation allocation);
			Builder(SingleAllocator& allocator);
			Builder(MultiAllocator& allocator, MemoryTypeCriteria memoryTypeCriteria);


			Builder& WithSize(size_t size) { mData = size; return *this; }
			Builder& WithData(const Core::IO::DynamicByteBuffer& bytes) { mData = bytes; return *this; }
			Builder& WithAlignment(size_t alignment) { mAlignment = alignment; return *this; }
			Builder& WithUsage(VkBufferUsageFlags usage) { mUsage = usage; return *this; }
			Builder& WithMemoryTypeCriteria(MemoryTypeCriteria criteria) { mMemoryTypeCriteria = criteria; return *this; }


			Buffer Build() const;


		private:
			const Device& GetDevice() const;
			size_t GetSize() const;
			Allocation AllocateMemory(const VkMemoryRequirements& requirements) const;


			mutable Core::Variant<Allocation, SingleAllocator*, MultiAllocator*> mAllocationSource;


			Core::Variant<size_t, Core::IO::DynamicByteBuffer> mData {static_cast<size_t>(0)};
			size_t mAlignment = 0;
			VkBufferUsageFlags mUsage = 0;
			MemoryTypeCriteria mMemoryTypeCriteria;
		};


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
		// Allocate a buffer with the given size and usage from the given allocator.
		Buffer(VkDevice device, size_t size, VkBufferUsageFlags usage);
		// Get the memory requiresments for this buffer.
		VkMemoryRequirements GetMemoryRequirements(VkDevice device) const;


		// The handle for this buffer.
		VkBuffer                          mHandle;
		// The amount of space allocated to this buffer.
		uint64_t                          mSize;
		// The memory allocated to this buffer.
		Allocation                        mMemory;
#ifdef STRAWBERRY_DEBUG
        // The usage flags for this buffer
        VkBufferUsageFlags mUsage;
#endif // STRAWBERRY_DEBUG
	};
}
