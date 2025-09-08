#include "MemoryPool.hpp"


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
		switch (vkAllocateMemory(device, &allocateInfo, nullptr, &address.deviceMemory))
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
		  , mSize(size) {}


	MemoryPool::MemoryPool(MemoryPool&& other) noexcept
		: EnableReflexivePointer(std::move(other))
		  , mDevice(std::move(other.mDevice))
		  , mMemoryTypeIndex(std::exchange(other.mMemoryTypeIndex, {}))
		  , mMemory(std::exchange(other.mMemory, VK_NULL_HANDLE))
		  , mSize(std::exchange(other.mSize, 0))
		  , mMappedAddress(std::move(other.mMappedAddress)) {}


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
		return { allocator, *this, offset, size };
	}

	Device& MemoryPool::GetDevice() const noexcept
	{
		return *mDevice;
	}

	VkDeviceMemory MemoryPool::Memory() const noexcept
	{
		return mMemory;
	}

	MemoryTypeIndex MemoryPool::GetMemoryTypeIndex() const noexcept
	{
		return mMemoryTypeIndex;
	}

	size_t MemoryPool::Size() const noexcept
	{
		return mSize;
	}


	VkMemoryPropertyFlags MemoryPool::Properties() const
	{
		return mMemoryTypeIndex.GetProperties();
	}


	uint8_t* MemoryPool::GetMappedAddress() const noexcept
	{
		if (!mMappedAddress) [[unlikely]]
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
}
