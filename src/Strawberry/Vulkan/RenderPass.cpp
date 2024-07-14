//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "RenderPass.hpp"
#include "Device.hpp"
// Standard Library
#include <memory>
#include <algorithm>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	RenderPass::RenderPass(const Device& device)
		: mDevice(device) {}


	RenderPass::RenderPass(RenderPass&& rhs) noexcept
		: mRenderPass(std::exchange(rhs.mRenderPass, nullptr))
		, mDevice(std::move(rhs.mDevice))
		, mColorAttachmentFormats(std::move(rhs.mColorAttachmentFormats))
		, mClearColors(std::move(rhs.mClearColors)) {}


	RenderPass& RenderPass::operator=(RenderPass&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	RenderPass::~RenderPass()
	{
		if (mRenderPass)
		{
			vkDestroyRenderPass(*mDevice, mRenderPass, nullptr);
		}
	}


	RenderPass::operator VkRenderPass() const noexcept
	{
		return mRenderPass;
	}


	Core::ReflexivePointer<Device> RenderPass::GetDevice() const
	{
		return mDevice;
	}


	SubpassDescription& SubpassDescription::WithInputAttachment(uint32_t index)
	{
		mInputAttachments.emplace_back(VkAttachmentReference{
			.attachment = index,
			.layout = VK_IMAGE_LAYOUT_GENERAL,
		});
		return *this;
	}


	SubpassDescription& SubpassDescription::WithColorAttachment(uint32_t index)
	{
		mColorAttachments.emplace_back(VkAttachmentReference{
			.attachment = index,
			.layout = VK_IMAGE_LAYOUT_GENERAL,
		});
		return *this;
	}


	SubpassDescription& SubpassDescription::WithDepthStencilAttachment(uint32_t index)
	{
		mDepthStencilAttachment = VkAttachmentReference{
			.attachment = index,
			.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
		};
		return *this;
	}


	RenderPass::Builder::Builder(const Device& device)
		: mDevice(device) {}


	RenderPass::Builder& RenderPass::Builder::WithColorAttachment(VkFormat            format,
	                                                              VkAttachmentLoadOp  loadOp,
	                                                              VkAttachmentStoreOp storeOp,
	                                                              VkImageLayout       initialLayout,
	                                                              VkImageLayout       finalLayout,
	                                                              Core::Math::Vec4f   clearColor,
	                                                              VkAttachmentLoadOp  stencilLoadOp,
	                                                              VkAttachmentStoreOp stencilStoreOp)
	{
		mAttachments.emplace_back(VkAttachmentDescription{
			.flags = 0,
			.format = format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = loadOp,
			.storeOp = storeOp,
			.stencilLoadOp = stencilLoadOp,
			.stencilStoreOp = stencilStoreOp,
			.initialLayout = initialLayout,
			.finalLayout = finalLayout,
		});

		mClearColors.emplace_back(VkClearValue{.color{.float32{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}}});

		return *this;
	}


	RenderPass::Builder& RenderPass::Builder::WithDepthAttachment(VkFormat            format,
	                                                              VkAttachmentLoadOp  loadOp,
	                                                              VkAttachmentStoreOp storeOp,
	                                                              VkImageLayout       initialLayout,
	                                                              VkImageLayout       finalLayout,
	                                                              float               clearValue,
	                                                              VkAttachmentLoadOp  stencilLoadOp,
	                                                              VkAttachmentStoreOp stencilStoreOp)
	{
		mAttachments.emplace_back(VkAttachmentDescription{
			.flags = 0,
			.format = format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = loadOp,
			.storeOp = storeOp,
			.stencilLoadOp = stencilLoadOp,
			.stencilStoreOp = stencilStoreOp,
			.initialLayout = initialLayout,
			.finalLayout = finalLayout,
		});
		mClearColors.emplace_back(VkClearValue{.depthStencil{.depth = clearValue}});
		return *this;
	}


	RenderPass::Builder& RenderPass::Builder::WithStencilAttachment(VkFormat            format,
	                                                                VkAttachmentLoadOp  loadOp,
	                                                                VkAttachmentStoreOp storeOp,
	                                                                VkImageLayout       initialLayout,
	                                                                VkImageLayout       finalLayout,
	                                                                uint32_t            clearValue,
	                                                                VkAttachmentLoadOp  stencilLoadOp,
	                                                                VkAttachmentStoreOp stencilStoreOp)
	{
		mAttachments.emplace_back(VkAttachmentDescription{
			.flags = 0,
			.format = format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = loadOp,
			.storeOp = storeOp,
			.stencilLoadOp = stencilLoadOp,
			.stencilStoreOp = stencilStoreOp,
			.initialLayout = initialLayout,
			.finalLayout = finalLayout,
		});
		mClearColors.emplace_back(VkClearValue{.depthStencil{.stencil = clearValue}});
		return *this;
	}


	RenderPass::Builder& RenderPass::Builder::WithSubpass(const SubpassDescription& subpass)
	{
		mSubpasses.emplace_back(subpass);
		return *this;
	}


	RenderPass::Builder RenderPass::Builder::WithSubpassDependency(uint32_t             srcSubpass,
	                                                               uint32_t             dstSubpass,
	                                                               VkPipelineStageFlags srcStageMask,
	                                                               VkPipelineStageFlags dstStageMask,
	                                                               VkAccessFlags        srcAccessMask,
	                                                               VkAccessFlags        dstAccessMask)
	{
		mDependencies.emplace_back(VkSubpassDependency{
			.srcSubpass = srcSubpass,
			.dstSubpass = dstSubpass,
			.srcStageMask = srcStageMask,
			.dstStageMask = dstStageMask,
			.srcAccessMask = srcAccessMask,
			.dstAccessMask = dstAccessMask,
			.dependencyFlags = 0,
		});
		return *this;
	}


	RenderPass RenderPass::Builder::Build()
	{
		RenderPass renderPass(*mDevice);
		std::transform(mAttachments.begin(),
		               mAttachments.end(),
		               std::back_inserter(renderPass.mColorAttachmentFormats),
		               [](const VkAttachmentDescription& x)
		               {
			               return x.format;
		               });
		renderPass.mClearColors = mClearColors;


		std::vector<VkSubpassDescription> mSubpassDescriptions;
		for (const SubpassDescription& subpass: mSubpasses)
		{
			mSubpassDescriptions.emplace_back(VkSubpassDescription{
				.flags = 0,
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount = static_cast<uint32_t>(subpass.mInputAttachments.size()),
				.pInputAttachments = subpass.mInputAttachments.data(),
				.colorAttachmentCount = static_cast<uint32_t>(subpass.mColorAttachments.size()),
				.pColorAttachments = subpass.mColorAttachments.data(),
				.pResolveAttachments = nullptr,
				.pDepthStencilAttachment = subpass.mDepthStencilAttachment.AsPtr().ValueOr(nullptr),
				.preserveAttachmentCount = 0,
				.pPreserveAttachments = nullptr,
			});
		}


		VkRenderPassCreateInfo renderPassCreateInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = static_cast<uint32_t>(mAttachments.size()),
			.pAttachments = mAttachments.data(),
			.subpassCount = static_cast<uint32_t>(mSubpassDescriptions.size()),
			.pSubpasses = mSubpassDescriptions.data(),
			.dependencyCount = static_cast<uint32_t>(mDependencies.size()),
			.pDependencies = mDependencies.data(),
		};
		Core::AssertEQ(vkCreateRenderPass(*mDevice, &renderPassCreateInfo, nullptr, &renderPass.mRenderPass),
		               VK_SUCCESS);


		return renderPass;
	}
}
