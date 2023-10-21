//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Buffer.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Buffer::Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage)
		: mSize(size)
		, mDevice(device.mDevice)
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

		mMemory = device.Create<DeviceMemory>(memoryRequirements.size, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		Core::AssertEQ(vkBindBufferMemory(mDevice, mBuffer, mMemory.mDeviceMemory, 0), VK_SUCCESS);
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


	void Buffer::SetData(const Core::IO::DynamicByteBuffer& bytes)
	{
		if (!mMappedDataPtr)
		{
			Core::AssertEQ(vkMapMemory(mDevice, mMemory.mDeviceMemory, 0, mSize, 0, &mMappedDataPtr), VK_SUCCESS);
			Core::AssertNEQ(mMappedDataPtr, nullptr);
		}
		std::memcpy(mMappedDataPtr, bytes.Data(), GetSize());

		VkMappedMemoryRange range {
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = mMemory.mDeviceMemory,
			.offset = 0,
			.size = mSize,
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(mDevice, 1, &range), VK_SUCCESS);
	}


	uint64_t Buffer::GetSize() const
	{
		return mSize;
	}
}
