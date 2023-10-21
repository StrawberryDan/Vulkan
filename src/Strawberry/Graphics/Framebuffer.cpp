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
	Framebuffer::Framebuffer(const Pipeline& pipeline, Core::Math::Vec2i size, uint32_t colorAttachmentCount)
		: mDevice(pipeline.mDevice)
		  , mSize(size)
		  , mDepthAttachment(CreateDepthImage(pipeline))
		  , mDepthAttachmentView(CreateDepthImageView())
		  , mStencilAttachment(CreateStencilImage(pipeline))
		  , mStencilAttachmentView(CreateStencilImageView())
	{
		for (int i = 0; i < colorAttachmentCount; i++)
		{
			mColorAttachments.emplace_back(pipeline.mDevice->Create<Image>(size, VK_FORMAT_R32G32B32A32_SFLOAT,
																		   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT));
			mColorAttachmentViews.emplace_back(
				mColorAttachments.back().Create<ImageView::Builder>()
					.WithType(VK_IMAGE_VIEW_TYPE_2D)
					.WithFormat(VK_FORMAT_R32G32B32A32_SFLOAT)
					.Build()
			);
		}


		std::vector<VkImageView> attachments;
		std::transform(mColorAttachmentViews.begin(), mColorAttachmentViews.end(),
					   std::back_inserter(attachments),
					   [](const ImageView& view) { return view.mImageView; });

		VkFramebufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.renderPass = pipeline.mRenderPass,
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.width = static_cast<uint32_t>(mSize[0]),
			.height = static_cast<uint32_t>(mSize[1]),
			.layers = 1,
		};
		Core::AssertEQ(vkCreateFramebuffer(mDevice->mDevice, &createInfo, nullptr, &mFramebuffer), VK_SUCCESS);
	}


	Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept
		: mFramebuffer(std::exchange(rhs.mFramebuffer, nullptr))
		  , mDevice(std::exchange(rhs.mDevice, nullptr))
		  , mSize(std::exchange(rhs.mSize, Core::Math::Vec2i()))
		  , mColorAttachments(std::move(rhs.mColorAttachments))
		  , mColorAttachmentViews(std::move(rhs.mColorAttachmentViews))
		  , mDepthAttachment(std::move(rhs.mDepthAttachment))
		  , mDepthAttachmentView(std::move(rhs.mDepthAttachmentView))
		  , mStencilAttachment(std::move(rhs.mStencilAttachment))
		  , mStencilAttachmentView(std::move(rhs.mStencilAttachmentView))
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
			vkDestroyFramebuffer(mDevice->mDevice, mFramebuffer, nullptr);
		}
	}


	uint32_t Framebuffer::GetColorAttachmentCount() const
	{
		return mColorAttachments.size();
	}


	const Image& Framebuffer::GetColorAttachment(uint32_t index)
	{
		return mColorAttachments[index];
	}


	const Image& Framebuffer::GetDepthAttachment()
	{
		return mDepthAttachment;
	}


	const Image& Framebuffer::GetStencilAttachment()
	{
		return mStencilAttachment;
	}


	Image Framebuffer::CreateDepthImage(const Pipeline& pipeline)
	{
		return pipeline.mDevice->Create<Image>(mSize, VK_FORMAT_D32_SFLOAT,
											   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

	}


	ImageView Framebuffer::CreateDepthImageView()
	{
		return mDepthAttachment.Create<ImageView::Builder>()
			.WithType(VK_IMAGE_VIEW_TYPE_2D)
			.WithFormat(VK_FORMAT_D32_SFLOAT)
			.WithSubresourceRange({
									  .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
									  .baseMipLevel = 0,
									  .levelCount = 1,
									  .baseArrayLayer = 0,
									  .layerCount = 1,})
			.Build();
	}


	Image Framebuffer::CreateStencilImage(const Pipeline& pipeline)
	{
		return pipeline.mDevice->Create<Image>(mSize, VK_FORMAT_S8_UINT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}


	ImageView Framebuffer::CreateStencilImageView()
	{
		return mStencilAttachment.Create<ImageView::Builder>()
			.WithType(VK_IMAGE_VIEW_TYPE_2D)
			.WithFormat(VK_FORMAT_S8_UINT)
			.WithSubresourceRange({
									  .aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT,
									  .baseMipLevel = 0,
									  .levelCount = 1,
									  .baseArrayLayer = 0,
									  .layerCount = 1,})
			.Build();
	}
}
