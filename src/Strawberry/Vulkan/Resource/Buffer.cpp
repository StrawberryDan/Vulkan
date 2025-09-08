//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Resource/Buffer.hpp"
#include "Strawberry/Vulkan/Device/Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Buffer::Builder::Builder(Device &device, MemoryTypeCriteria memoryTypeCriteria)
		: Builder(device.GetAllocator(), memoryTypeCriteria)
	{}

	Buffer::Builder::Builder(MemoryBlock allocation)
		: mAllocationSource(std::move(allocation))
	{}

	Buffer::Builder::Builder(SingleAllocator& allocator)
		: mAllocationSource(&allocator)
	{}

	const Device& Buffer::Builder::GetDevice() const
	{
		return mAllocationSource.Visit(
			[](MemoryBlock& allocation) -> const Device&
			{
				return allocation.GetDevice();
			},
			[](SingleAllocator* allocator) -> const Device&
			{
				return allocator->GetDevice();
			},
			[](MultiAllocator* allocator) -> const Device&
			{
				return allocator->GetDevice();
			});
	}

	size_t Buffer::Builder::GetSize() const
	{
		return mData.Visit(
			[](const Core::IO::DynamicByteBuffer& bytes)
			{
				return bytes.Size();
			},
			[](size_t size)
			{
				return size;
			});
	}

	MemoryBlock Buffer::Builder::AllocateMemory(const VkMemoryRequirements& requirements) const
	{
		return mAllocationSource.Visit(
			[&](MemoryBlock& allocation)
			{
				return std::move(allocation);
			},
			[&](SingleAllocator* allocator)
			{
				return allocator->Allocate( AllocationRequest(requirements)).Unwrap();
			},
			[&](MultiAllocator* allocator)
			{
				return allocator->Allocate(AllocationRequest(requirements), mMemoryTypeCriteria).Unwrap();
			}
		);
	}


	Buffer::Builder::Builder(MultiAllocator& allocator, MemoryTypeCriteria memoryTypeCriteria)
		: mAllocationSource(&allocator)
		, mMemoryTypeCriteria(memoryTypeCriteria)
	{}


	Buffer Buffer::Builder::Build() const
	{
		// Create Buffer
		Buffer buffer {GetDevice(), GetSize(), mUsage};
		// Get memory requirements
		VkMemoryRequirements memoryRequirements = buffer.GetMemoryRequirements(GetDevice());
		// Allocate memory
		buffer.mMemory = AllocateMemory(memoryRequirements);
		// Bind memory to buffer
		Core::AssertEQ(
			vkBindBufferMemory(
				GetDevice(),
				buffer.mHandle,
				buffer.mMemory.Memory(),
				buffer.mMemory.Offset()),
			VK_SUCCESS);

		// Copy data into the buffer if required.
		if (mData.IsType<Core::IO::DynamicByteBuffer>())
		{
			Core::Assert(mUsage & VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			buffer.SetData(mData.Ref<Core::IO::DynamicByteBuffer>());
		}

		return buffer;
	}


	Buffer::Buffer(Buffer&& rhs) noexcept
		: mSize(std::exchange(rhs.mSize, 0))
		, mHandle(std::exchange(rhs.mHandle, nullptr))
		, mMemory(std::move(rhs.mMemory))
#ifdef STRAWBERRY_DEBUG
        , mUsage(std::exchange(rhs.mUsage, 0))
#endif
	{}


	Buffer& Buffer::operator=(Buffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Buffer::~Buffer()
	{
		if (mHandle)
		{
			vkDestroyBuffer(GetDevice(), mHandle, nullptr);
		}
	}


	Buffer::operator VkBuffer() const
	{
		return mHandle;
	}


	VkDevice Buffer::GetDevice() const
	{
		return mMemory.GetDevice();
	}


	void Buffer::SetData(const Core::IO::DynamicByteBuffer& bytes)
	{
		mMemory.Overwrite(bytes);
	}

	uint8_t* Buffer::GetData()
	{
		return mMemory.GetMappedAddress();
	}

	const uint8_t* Buffer::GetData() const
	{
		return mMemory.GetMappedAddress();
	}

	uint64_t Buffer::GetSize() const
	{
		return mSize;
	}

	Buffer::Buffer(VkDevice device, size_t size, VkBufferUsageFlags usage)
		: mHandle(VK_NULL_HANDLE)
		, mSize(size)
#ifdef STRAWBERRY_DEBUG
		, mUsage(usage)
#endif
{
		VkBufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
		};

		Core::AssertEQ(vkCreateBuffer(device, &createInfo, nullptr, &mHandle), VK_SUCCESS);
	}

	VkMemoryRequirements Buffer::GetMemoryRequirements(VkDevice device) const
	{
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, mHandle, &memoryRequirements);
		return memoryRequirements;
	}
}
