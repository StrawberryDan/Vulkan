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


	Allocation::Allocation(Allocator& allocator, GPUMemoryRange memoryRange, VkMemoryPropertyFlags memoryType)
		: mAllocator(allocator)
		, mRange(memoryRange)
		, mMemoryType(memoryType) {}


	Allocation::Allocation(Allocation&& other) noexcept
		: mAllocator(std::move(other.mAllocator))
		, mRange(other.mRange)
		, mMemoryType(std::exchange(other.mMemoryType, {})) {}


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
		Core::Assert(mMemoryType & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		void* mappedAddress = nullptr;
		Core::AssertEQ(vkMapMemory(*mAllocator->GetDevice(), mRange.address.deviceMemory, mRange.address.offset, mRange.size, 0, &mappedAddress), VK_SUCCESS);
		mMappedAddress = static_cast<uint8_t*>(mappedAddress);
		return mMappedAddress.Value();
	}


	void Allocation::Flush() const noexcept
	{
		VkMappedMemoryRange range
		{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = mRange.address.deviceMemory,
			.offset = mRange.address.offset,
			.size = mRange.size
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(*mAllocator->GetDevice(), 1, &range), VK_SUCCESS);
	}


	void Allocation::Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept
	{
		Core::Assert(bytes.Size() <= Size());
		std::memcpy(GetMappedAddress(), bytes.Data(), bytes.Size());

		if (!(mMemoryType & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			Flush();
		}
	}
}
