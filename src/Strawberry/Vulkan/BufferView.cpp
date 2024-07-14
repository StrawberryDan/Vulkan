//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "BufferView.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	BufferView::BufferView(const Buffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize range)
		: mDevice(buffer.GetDevice())
	{
		VkBufferViewCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.buffer = buffer,
			.format = format,
			.offset = offset,
			.range = range
		};
		Core::AssertEQ(vkCreateBufferView(mDevice, &createInfo, nullptr, &mBufferView), VK_SUCCESS);
	}


	BufferView::BufferView(BufferView&& rhs) noexcept
		: mBufferView(std::exchange(rhs.mBufferView, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr)) {}


	BufferView& BufferView::operator=(BufferView&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	BufferView::~BufferView()
	{
		if (mBufferView)
		{
			vkDestroyBufferView(mDevice, mBufferView, nullptr);
		}
	}
}
