//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Framebuffer.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "ImageView.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Framebuffer::Framebuffer(const Pipeline& pipeline, const ImageView& colorAttachment)
		: mDevice(pipeline.mDevice)
		, mSize(colorAttachment.mSize)
	{
		VkFramebufferCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.renderPass = pipeline.mRenderPass,
			.attachmentCount = 1,
			.pAttachments = &colorAttachment.mImageView,
			.width = static_cast<uint32_t>(colorAttachment.mSize[0]),
			.height = static_cast<uint32_t>(colorAttachment.mSize[1]),
			.layers = 1,
		};
		Core::AssertEQ(vkCreateFramebuffer(mDevice, &createInfo, nullptr, &mFramebuffer), VK_SUCCESS);
	}


	Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept
		: mFramebuffer(std::exchange(rhs.mFramebuffer, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
		, mSize(std::exchange(rhs.mSize, Core::Math::Vec2i()))
	{

	}


	Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Framebuffer::~Framebuffer()
	{
		if (mFramebuffer)
		{
			vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
		}
	}
}
