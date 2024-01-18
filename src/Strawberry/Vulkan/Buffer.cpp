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
	Buffer::Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage)
		: mSize(size)
		, mDevice(device)
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

		mMemory = DeviceMemory(device, memoryRequirements.size, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		Core::AssertEQ(vkBindBufferMemory(mDevice, mBuffer, mMemory.mDeviceMemory, 0), VK_SUCCESS);
	}


	Buffer::Buffer(const Device& device, const Core::IO::DynamicByteBuffer& bytes, VkBufferUsageFlags usage)
		: Buffer(device, bytes.Size(), usage)
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
		mBytes = bytes;
		mMemory.SetData(GetBytes());
	}


	uint64_t Buffer::GetSize() const
	{
		return mSize;
	}


	Core::IO::DynamicByteBuffer Buffer::GetBytes() const
	{
		return mBytes;
	}
}
