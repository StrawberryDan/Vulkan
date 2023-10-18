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
		: mDevice(device.mDevice)
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
	}


	Buffer::Buffer(Buffer&& rhs) noexcept
		: mBuffer(std::exchange(rhs.mBuffer, nullptr))
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
}
