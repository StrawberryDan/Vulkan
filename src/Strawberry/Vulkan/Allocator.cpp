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
		: mDevice(device)
	{}


	Core::ReflexivePointer<Device> Allocator::GetDevice() const noexcept
	{
		return mDevice;
	}


	VkMemoryPropertyFlags Allocator::GetMemoryProperties(VkDeviceMemory memory) const noexcept
	{
		Core::Assert(mMemoryProperties.contains(memory));
		return mMemoryProperties.at(memory);
	}


	bool Allocator::MemoryHasProperty(VkDeviceMemory memory, VkMemoryPropertyFlags properties) const noexcept
	{
		return (GetMemoryProperties(memory) & properties) == properties;
	}


	Core::Optional<uint8_t*> Allocator::MapMemory(VkDeviceMemory memory) const noexcept
	{
		if (mMemoryMappings.contains(memory))
		{
			return mMemoryMappings.at(memory);
		}

		void* addr = nullptr;
		Core::AssertEQ(vkMapMemory(*GetDevice(), memory, 0, VK_WHOLE_SIZE, 0, &addr), VK_SUCCESS);
		uint8_t* castAddr = reinterpret_cast<uint8_t*>(addr);

		mMemoryMappings.emplace(memory, castAddr);
		return castAddr;
	}


	void Allocator::UnmapMemory(VkDeviceMemory memory) const noexcept
	{
		vkUnmapMemory(*GetDevice(), memory);
	}


	bool Allocator::IsMemoryMapped(VkDeviceMemory memory) const noexcept
	{
		return mMemoryMappings.contains(memory);
	}


	void Allocator::FlushMappedMemory(VkDeviceMemory memory) const noexcept
	{
		VkMappedMemoryRange range
		{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = memory,
			.offset = 0,
			.size = VK_WHOLE_SIZE
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(*GetDevice(), 1, &range), VK_SUCCESS);
	}


	Allocation::Allocation(Allocator& allocator, GPUMemoryRange memoryRange)
		: mAllocator(allocator)
		, mRange(memoryRange)
	{}


	Allocation::Allocation(Allocation&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		, mRange(other.mRange)
	{}


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
		if (mAllocator) mAllocator->Free(mRange.address);
	}


	const GPUAddress& Allocation::Address() const noexcept
	{
		return mRange.address;
	}


	const size_t Allocation::Size() const noexcept
	{
		return mRange.size;
	}


	uint8_t* Allocation::GetMappedAddress() const noexcept
	{
		return mAllocator->MapMemory(mRange.address.deviceMemory).Unwrap() + mRange.address.offset;
	}


	void Allocation::Flush() const noexcept
	{
		mAllocator->FlushMappedMemory(mRange.address.deviceMemory);
	}


	void Allocation::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());

		if (!mAllocator->MemoryHasProperty(mRange.address.deviceMemory, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			Flush();
		}
	}
}