//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Buffer.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>
#include <cstring>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Buffer::Buffer(Allocator* allocator, const MemoryTypeCriteria& criteria, size_t size, VkBufferUsageFlags usage)
		: mSize(size)
		, mDevice(*allocator->GetDevice())
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

		Core::AssertEQ(vkCreateBuffer(mDevice, &createInfo, nullptr, &mBuffer), VK_SUCCESS);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(mDevice, mBuffer, &memoryRequirements);


		mMemory = allocator->Allocate(memoryRequirements.size, criteria).Unwrap();
		Core::AssertEQ(vkBindBufferMemory(mDevice, mBuffer, mMemory.Memory(), mMemory.Offset()), VK_SUCCESS);
	}


	Buffer::Buffer(Allocator*                         allocator,
	               const MemoryTypeCriteria&          memoryCriteria,
	               const Core::IO::DynamicByteBuffer& bytes,
	               VkBufferUsageFlags                 usage)
		: Buffer(allocator, memoryCriteria, bytes.Size(), usage)
	{
		Core::Assert(usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		SetData(bytes);
	}


	Buffer::Buffer(Buffer&& rhs) noexcept
		: mBuffer(std::exchange(rhs.mBuffer, nullptr))
		, mMemory(std::move(rhs.mMemory))
		, mDevice(std::exchange(rhs.mDevice, nullptr)) {}


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
		if (mBuffer)
		{
			vkDestroyBuffer(mDevice, mBuffer, nullptr);
		}
	}


	Buffer::operator VkBuffer() const
	{
		return mBuffer;
	}


	VkDevice Buffer::GetDevice() const
	{
		return mDevice;
	}


	void Buffer::SetData(const Core::IO::DynamicByteBuffer& bytes)
	{
		Core::AssertEQ(bytes.Size(), GetSize());
		mMemory.Overwrite(bytes);
	}


	uint64_t Buffer::GetSize() const
	{
		return mSize;
	}
}
