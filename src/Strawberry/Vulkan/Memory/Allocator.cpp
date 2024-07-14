//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Allocator.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Allocator::Allocator(Device& device)
		: mDevice(device) {}


	void Allocator::Free(Allocation&& allocation) const
	{
		vkFreeMemory(*GetDevice(), allocation.Memory(), nullptr);
	}


	Core::ReflexivePointer<Device> Allocator::GetDevice() const noexcept
	{
		return mDevice;
	}


	Allocation::Allocation(Allocator& allocator, VkDeviceMemory memory, size_t size, VkMemoryPropertyFlags memoryType)
		: mAllocator(allocator)
		, mMemory(memory)
		, mSize(size)
		, mMemoryProperties(memoryType) {}


	Allocation::Allocation(Allocation&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		, mMemory(std::exchange(other.mMemory, VK_NULL_HANDLE))
		, mSize(std::exchange(other.mSize, 0))
		, mMemoryProperties(std::exchange(other.mMemoryProperties, {})) {}


	Allocation& Allocation::operator=(Allocation&& other) noexcept
	{
		if (this != &other)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}


	Allocation::~Allocation()
	{
		if (mAllocator) mAllocator->Free(std::move(*this));
	}


	AllocationView Allocation::AllocateView(size_t offset, size_t size)
	{
		return {*this, offset, size};
	}


	Core::ReflexivePointer<Allocator> Allocation::GetAllocator() const noexcept
	{
		return mAllocator;
	}


	VkDeviceMemory Allocation::Memory() const noexcept
	{
		return mMemory;
	}


	size_t Allocation::Size() const noexcept
	{
		return mSize;
	}


	VkMemoryPropertyFlags Allocation::Properties() const
	{
		return mMemoryProperties;
	}


	uint8_t* Allocation::GetMappedAddress() const noexcept
	{
		Core::Assert(mMemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		void* mappedAddress = nullptr;
		Core::AssertEQ(vkMapMemory(*mAllocator->GetDevice(), mMemory, 0, VK_WHOLE_SIZE, 0, &mappedAddress), VK_SUCCESS);
		mMappedAddress = static_cast<uint8_t*>(mappedAddress);
		return mMappedAddress.Value();
	}


	void Allocation::Flush() const noexcept
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


	void Allocation::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());

		if (!(mMemoryProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			Flush();
		}
	}


	AllocationView::AllocationView(Allocation& allocation, size_t offset, size_t size)
		: mAllocation(allocation)
		, mOffset(offset)
		, mSize(size) {}


	Core::ReflexivePointer<Allocator> AllocationView::GetAllocator() const noexcept
	{
		return mAllocation->GetAllocator();
	}


	VkDeviceMemory AllocationView::Memory() const noexcept
	{
		return mAllocation->Memory();
	}


	size_t AllocationView::Offset() const noexcept
	{
		return mOffset;
	}


	size_t AllocationView::Size() const noexcept
	{
		return mSize;
	}


	VkMemoryPropertyFlags AllocationView::Properties() const
	{
		return mAllocation->Properties();
	}


	uint8_t* AllocationView::GetMappedAddress() const noexcept
	{
		return mAllocation->GetMappedAddress() + mOffset;
	}


	void AllocationView::Flush() const noexcept
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


	void AllocationView::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());
	}
}
