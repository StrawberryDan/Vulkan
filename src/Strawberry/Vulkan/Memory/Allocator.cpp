//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Allocator.hpp"
#include "Allocation.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Allocator::Allocator(Device& device)
		: mDevice(device)
	{}

	Allocation::Allocation(Allocator& allocator,
		                   MemoryPool& allocation,
		                   size_t offset,
	                       size_t size)
		: mAllocator(allocator)
		  , mMemoryPool(allocation)
		  , mOffset(offset)
		  , mSize(size)
	{}


	Allocation::Allocation(Allocation&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		, mMemoryPool(std::move(other.mMemoryPool))
		, mOffset(other.mOffset)
		, mSize(other.mSize)
	{}


	Allocation& Allocation::operator=(Allocation&& other) noexcept
	{
		if (this != &other) [[likely]]
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}
		return *this;
	}


	Allocation::~Allocation()
	{
		Core::AssertImplication(!mAllocator, !mMemoryPool);
		if (mAllocator)
		{
			auto allocator = mAllocator.Get();
			allocator->Free(std::move(*this));
		}
	}

	Allocation::operator bool() const noexcept
	{
		Core::AssertImplication(!mAllocator, mMemoryPool);
		return mAllocator;
	}

	Device & Allocation::GetDevice() noexcept
	{
		return mAllocator->GetDevice();
	}


	const Device& Allocation::GetDevice() const noexcept
	{
		return mAllocator->GetDevice();
	}

	Core::ReflexivePointer<Allocator> Allocation::GetAllocator() const noexcept
	{
		return mAllocator;
	}


	Address Allocation::Address() const noexcept
	{
		return {
			.deviceMemory = mMemoryPool->Memory(),
			.offset = Offset()
		};
	}


	VkDeviceMemory Allocation::Memory() const noexcept
	{
		return mMemoryPool->Memory();
	}


	size_t Allocation::Offset() const noexcept
	{
		return mOffset;
	}


	size_t Allocation::Size() const noexcept
	{
		return mSize;
	}


	VkMemoryPropertyFlags Allocation::Properties() const
	{
		return mMemoryPool->Properties();
	}


	uint8_t* Allocation::GetMappedAddress() const noexcept
	{
		return mMemoryPool->GetMappedAddress() + mOffset;
	}


	void Allocation::Flush() const noexcept
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


	void Allocation::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());
		if (!(Properties() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			Flush();
		}
	}
}
