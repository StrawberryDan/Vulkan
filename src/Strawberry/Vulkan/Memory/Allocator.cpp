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


	VkDeviceMemory Allocation::Memory() const noexcept
	{
		return mMemory;
	}


	Address Allocation::Address() const noexcept
	{
		return {mMemory, 0};
	}


	size_t Allocation::Size() const noexcept
	{
		return mSize;
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
}
