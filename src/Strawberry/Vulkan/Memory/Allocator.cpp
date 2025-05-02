//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Allocator.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Core::Result<MemoryPool, AllocationError> MemoryPool::Allocate(Device& device, MemoryTypeIndex memoryTypeIndex,
																   size_t size)
	{
		const VkMemoryAllocateInfo allocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = size,
			.memoryTypeIndex = memoryTypeIndex.memoryTypeIndex,
		};

		Address address;
		switch (VkResult allocationResult = vkAllocateMemory(device, &allocateInfo, nullptr, &address.deviceMemory))
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return AllocationError::OutOfMemory();
		case VK_SUCCESS:
			break;
		default:
			Core::Unreachable();
		}

		return MemoryPool(device, memoryTypeIndex, address.deviceMemory, size);
	}


	MemoryPool::MemoryPool(Device& device, MemoryTypeIndex memoryTypeIndex, VkDeviceMemory memory, size_t size)
		: mDevice(device)
		  , mMemoryTypeIndex(memoryTypeIndex)
		  , mMemory(memory)
		  , mSize(size)
	{
	}


	MemoryPool::MemoryPool(MemoryPool&& other) noexcept
		: EnableReflexivePointer(std::move(other))
		  , mDevice(std::move(other.mDevice))
		  , mMemoryTypeIndex(std::exchange(other.mMemoryTypeIndex, {}))
		  , mMemory(std::exchange(other.mMemory, VK_NULL_HANDLE))
		  , mSize(std::exchange(other.mSize, 0))
		  , mMappedAddress(std::move(other.mMappedAddress))
	{
	}


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
		if (mMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(*mDevice, mMemory, nullptr);
		}
	}


	Allocation MemoryPool::AllocateView(Allocator& allocator, size_t offset, size_t size)
	{
		return {*mDevice, allocator, *this, offset, size};
	}


	VkMemoryPropertyFlags MemoryPool::Properties() const
	{
		return mMemoryTypeIndex.GetProperties();
	}


	uint8_t* MemoryPool::GetMappedAddress() const noexcept
	{
		if (!mMappedAddress)
		[[unlikely]]
		{
			Core::Assert(Properties() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			void* mappedAddress = nullptr;
			Core::AssertEQ(vkMapMemory(*mDevice, mMemory, 0, VK_WHOLE_SIZE, 0, &mappedAddress), VK_SUCCESS);
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
		Core::AssertEQ(vkFlushMappedMemoryRanges(*mDevice, 1, &range), VK_SUCCESS);
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


	Allocator::Allocator(Device& device)
		: mDevice(device)
	{}


	Allocation::Allocation(const Device& device, Allocator& allocator, MemoryPool& allocation, size_t offset,
						   size_t size)
		: mDevice(device)
		  , mAllocator(allocator)
		  , mRawAllocation(allocation)
		  , mOffset(offset)
		  , mSize(size)
	{
	}


	Allocation::Allocation(Allocation&& other) noexcept
		: mDevice(std::exchange(other.mDevice, VK_NULL_HANDLE))
		  , mAllocator(std::move(other.mAllocator))
		  , mRawAllocation(std::move(other.mRawAllocation))
		  , mOffset(other.mOffset)
		  , mSize(other.mSize)
	{
	}


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
		return mAllocator;
	}


	Address Allocation::Address() const noexcept
	{
		return {
			.deviceMemory = mRawAllocation->Memory(),
			.offset = Offset()
		};
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
		Core::AssertEQ(vkFlushMappedMemoryRanges(mDevice, 1, &range), VK_SUCCESS);
	}


	void Allocation::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());
	}
}
