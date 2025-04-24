//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Framebuffer.hpp"
#include "Device.hpp"
#include "GraphicsPipeline.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <algorithm>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Framebuffer::Framebuffer(const RenderPass& renderPass, Allocator& allocator, Core::Math::Vec2u size)
		: mRenderPass(renderPass)
		  , mSize(size)
	{
		const auto ATTACHMENT_COUNT = mRenderPass->mAttachmentFormats.size();
		for (int i = 0; i < ATTACHMENT_COUNT; i++)
		{
			Image image(
				allocator,
				mSize,
				renderPass.mAttachmentFormats[i],
				renderPass.mAttachmentUsages[i]);

			mAttachments.emplace_back(std::move(image));

			VkImageAspectFlags aspectFlags = [&]()
			{
				switch (renderPass.mAttachmentFormats[i])
				{
				case VK_FORMAT_D16_UNORM:
				case VK_FORMAT_D32_SFLOAT:
				case VK_FORMAT_D16_UNORM_S8_UINT:
				case VK_FORMAT_D24_UNORM_S8_UINT:
				case VK_FORMAT_D32_SFLOAT_S8_UINT:
					return VK_IMAGE_ASPECT_DEPTH_BIT;
				default:
					return VK_IMAGE_ASPECT_COLOR_BIT;;
				}
			}();

			ImageView view = mAttachments.back().Create<ImageView::Builder>(aspectFlags)
										 .WithType(VK_IMAGE_VIEW_TYPE_2D)
										 .WithFormat(renderPass.mAttachmentFormats[i])
										 .Build();

			mAttachmentViews.emplace_back(std::move(view));
		}


		std::vector<VkImageView> attachments;
		std::transform(mAttachmentViews.begin(),
					   mAttachmentViews.end(),
					   std::back_inserter(attachments),
					   [](const ImageView& view) -> VkImageView
					   {
						   return view;
					   });

		VkFramebufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.renderPass = mRenderPass->mRenderPass,
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.width = mSize[0],
			.height = mSize[1],
			.layers = 1,
		};
		Core::AssertEQ(vkCreateFramebuffer(*mRenderPass->mDevice, &createInfo, nullptr, &mFramebuffer), VK_SUCCESS);
	}


	Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept
		: mFramebuffer(std::exchange(rhs.mFramebuffer, nullptr))
		  , mRenderPass(std::move(rhs.mRenderPass))
		  , mSize(std::exchange(rhs.mSize, Core::Math::Vec2u()))
		  , mAttachments(std::move(rhs.mAttachments))
		  , mAttachmentViews(std::move(rhs.mAttachmentViews))
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
			vkDestroyFramebuffer(*mRenderPass->mDevice, mFramebuffer, nullptr);
		}
	}


	Framebuffer::operator VkFramebuffer() const noexcept
	{
		return mFramebuffer;
	}


	Core::ReflexivePointer<RenderPass> Framebuffer::GetRenderPass() const
	{
		return mRenderPass;
	}

	Core::ReflexivePointer<Device> Framebuffer::GetDevice() const
	{
		return GetRenderPass()->GetDevice();
	}

	Core::ReflexivePointer<PhysicalDevice> Framebuffer::GetPhysicalDevice() const
	{
		return GetDevice()->GetPhysicalDevice().GetReflexivePointer();
	}

	Core::Math::Vec2u Framebuffer::GetSize() const
	{
		return mSize;
	}


	uint32_t Framebuffer::GetAttachmentCount() const
	{
		return mAttachments.size();
	}


	Image& Framebuffer::GetAttachment(uint32_t index)
	{
		return mAttachments[index];
	}
}
