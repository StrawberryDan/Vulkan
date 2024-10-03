//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Allocator.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	AllocationRequest::AllocationRequest(VkMemoryRequirements& requirements)
		: size(requirements.size)
		, alignment(requirements.alignment) {}


	Allocator::Allocator(Device& device, uint32_t memoryType)
		: mDevice(device)
		, mMemoryType(memoryType) {}


	void Allocator::Free(MemoryPool&& allocation) const
	{
		vkFreeMemory(*GetDevice(), allocation.Memory(), nullptr);
	}


	Core::ReflexivePointer<Device> Allocator::GetDevice() const noexcept
	{
		return mDevice;
	}


	MemoryPool::MemoryPool(Allocator& allocator, VkDeviceMemory memory, size_t size)
		: mAllocator(allocator)
		, mMemory(memory)
		, mSize(size) {}


	MemoryPool::MemoryPool(MemoryPool&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		, mMemory(std::exchange(other.mMemory, VK_NULL_HANDLE))
		, mSize(std::exchange(other.mSize, 0)) {}


	MemoryPool& MemoryPool::operator=(MemoryPool&& other) noexcept
	{
		if (this != &other)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}


	MemoryPool::~MemoryPool()
	{
		if (mAllocator) mAllocator->Free(std::move(*this));
	}


	Allocation MemoryPool::AllocateView(Allocator& allocator, size_t offset, size_t size)
	{
		return {allocator, *this, offset, size};
	}


	Core::ReflexivePointer<Allocator> MemoryPool::GetAllocator() const noexcept
	{
		return mAllocator;
	}


	VkDeviceMemory MemoryPool::Memory() const noexcept
	{
		return mMemory;
	}


	size_t MemoryPool::Size() const noexcept
	{
		return mSize;
	}


	VkMemoryPropertyFlags MemoryPool::Properties() const
	{
		return mAllocator->GetDevice()->GetPhysicalDevices()[0]->GetMemoryProperties().memoryTypes[mAllocator->MemoryType()].propertyFlags;
	}


	uint8_t* MemoryPool::GetMappedAddress() const noexcept
	{
		if (!mMappedAddress)
		[[unlikely]]
		{
			Core::Assert(Properties() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			void* mappedAddress = nullptr;
			Core::AssertEQ(vkMapMemory(*mAllocator->GetDevice(), mMemory, 0, VK_WHOLE_SIZE, 0, &mappedAddress), VK_SUCCESS);
			mMappedAddress = static_cast<uint8_t*>(mappedAddress);
		}
		return mMappedAddress.Value();
	}


	void MemoryPool::Flush() const noexcept
	{
		VkMappedMemoryRange range
		{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = mMemory,
			.offset = 0,
			.size = VK_WHOLE_SIZE
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(*mAllocator->GetDevice(), 1, &range), VK_SUCCESS);
	}


	void MemoryPool::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());

		if (!(Properties() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			Flush();
		}
	}


	Allocation::Allocation(Allocator& allocator, MemoryPool& allocation, size_t offset, size_t size)
		: mAllocator(allocator)
		, mRawAllocation(allocation)
		, mOffset(offset)
		, mSize(size) {}


	Allocation::Allocation(Allocation&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		, mRawAllocation(std::move(other.mRawAllocation))
		, mOffset(other.mOffset)
		, mSize(other.mSize) {}


	Allocation& Allocation::operator=(Allocation&& other) noexcept
	{
		if (this != &other)
		[[likely]]
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}
		return *this;
	}


	Allocation::~Allocation()
	{
		Core::AssertImplication(!mAllocator, !mRawAllocation);
		if (mAllocator)
		{
			mAllocator->Free(std::move(*this));
		}
	}


	Core::ReflexivePointer<Allocator> Allocation::GetAllocator() const noexcept
	{
		return mRawAllocation->GetAllocator();
	}


	VkDeviceMemory Allocation::Memory() const noexcept
	{
		return mRawAllocation->Memory();
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
		return mRawAllocation->Properties();
	}


	uint8_t* Allocation::GetMappedAddress() const noexcept
	{
		return mRawAllocation->GetMappedAddress() + mOffset;
	}


	void Allocation::Flush() const noexcept
	{
		VkMappedMemoryRange range
		{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = Memory(),
			.offset = Offset(),
			.size = Size()
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(*GetAllocator()->GetDevice(), 1, &range), VK_SUCCESS);
	}


	void Allocation::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());
	}
}
