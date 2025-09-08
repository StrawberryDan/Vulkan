//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Allocator.hpp"
#include "Strawberry/Vulkan/Memory/MemoryBlock.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Allocator::Allocator(Device& device)
		: mDevice(device) {}

	MemoryBlock::MemoryBlock(Allocator&  allocator,
						   MemoryPool& allocation,
						   size_t      offset,
						   size_t      size)
		: mAllocator(allocator)
		  , mMemoryPool(allocation)
		  , mOffset(offset)
		  , mSize(size) {}


	MemoryBlock::MemoryBlock(MemoryBlock&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		  , mMemoryPool(std::move(other.mMemoryPool))
		  , mOffset(other.mOffset)
		  , mSize(other.mSize) {}


	MemoryBlock& MemoryBlock::operator=(MemoryBlock&& other) noexcept
	{
		if (this != &other) [[likely]]
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}
		return *this;
	}


	MemoryBlock::~MemoryBlock()
	{
		Core::AssertImplication(!mAllocator, !mMemoryPool);
		if (mAllocator)
		{
			auto allocator = mAllocator.Get();
			allocator->Free(std::move(*this));
		}
	}

	MemoryBlock::operator bool() const noexcept
	{
		Core::AssertImplication(!mAllocator, mMemoryPool);
		return mAllocator;
	}

	Device& MemoryBlock::GetDevice() noexcept
	{
		return mAllocator->GetDevice();
	}


	const Device& MemoryBlock::GetDevice() const noexcept
	{
		return mAllocator->GetDevice();
	}

	Core::ReflexivePointer<Allocator> MemoryBlock::GetAllocator() const noexcept
	{
		return mAllocator;
	}


	Address MemoryBlock::Address() const noexcept
	{
		return {
			.deviceMemory = mMemoryPool->Memory(),
			.offset = Offset()
		};
	}


	VkDeviceMemory MemoryBlock::Memory() const noexcept
	{
		return mMemoryPool->Memory();
	}


	size_t MemoryBlock::Offset() const noexcept
	{
		return mOffset;
	}


	size_t MemoryBlock::Size() const noexcept
	{
		return mSize;
	}


	VkMemoryPropertyFlags MemoryBlock::Properties() const
	{
		return mMemoryPool->Properties();
	}


	uint8_t* MemoryBlock::GetMappedAddress() const noexcept
	{
		return mMemoryPool->GetMappedAddress() + mOffset;
	}


	void MemoryBlock::Flush() const noexcept
	{
		VkMappedMemoryRange range
		{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = Memory(),
			.offset = Offset(),
			.size = VK_WHOLE_SIZE
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(mAllocator->GetDevice(), 1, &range), VK_SUCCESS);
	}


	void MemoryBlock::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());
		if (!(Properties() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			Flush();
		}
	}
}
