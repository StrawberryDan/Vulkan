#pragma once

#include "Memory.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"


namespace Strawberry::Vulkan
{
	class Allocator;
	class MemoryPool;
	class Device;


	class MemoryBlock
	{
	public:
		MemoryBlock() = default;
		MemoryBlock(Allocator& allocator, MemoryPool& allocation, size_t offset, size_t size);
		MemoryBlock(const MemoryBlock&)            = delete;
		MemoryBlock& operator=(const MemoryBlock&) = delete;
		MemoryBlock(MemoryBlock&& other) noexcept;
		MemoryBlock& operator=(MemoryBlock&& other) noexcept;
		~MemoryBlock();


		explicit operator bool() const noexcept;


		[[nodiscard]]       Device&                     GetDevice()       noexcept;
		[[nodiscard]] const Device&                     GetDevice() const noexcept;
		[[nodiscard]] Core::ReflexivePointer<Allocator> GetAllocator() const noexcept;
		[[nodiscard]] Address                           Address() const noexcept;
		[[nodiscard]] VkDeviceMemory                    Memory() const noexcept;
		[[nodiscard]] size_t                            Offset() const noexcept;
		[[nodiscard]] size_t                            Size() const noexcept;
		[[nodiscard]] VkMemoryPropertyFlags             Properties() const;
		[[nodiscard]] uint8_t*                          GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;


	private:
		Core::ReflexivePointer<Allocator>  mAllocator  = nullptr;
		Core::ReflexivePointer<MemoryPool> mMemoryPool = nullptr;
		size_t                             mOffset     = 0;
		size_t                             mSize       = 0;
	};
}