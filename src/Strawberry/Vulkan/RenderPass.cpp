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
		, mAttachmentFormats(std::move(rhs.mAttachmentFormats))
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


	SubpassDescription& SubpassDescription::WithInputAttachment(uint32_t index, VkImageLayout layout)
	{
		mInputAttachments.emplace_back(VkAttachmentReference{
			.attachment = index,
			.layout = layout,
		});
		return *this;
	}


	SubpassDescription& SubpassDescription::WithColorAttachment(uint32_t index, VkImageLayout layout)
	{
		mColorAttachments.emplace_back(VkAttachmentReference{
			.attachment = index,
			.layout = layout,
		});
		return *this;
	}


	SubpassDescription& SubpassDescription::WithDepthStencilAttachment(uint32_t index, VkImageLayout layout)
	{
		mDepthStencilAttachment = VkAttachmentReference{
			.attachment = index,
			.layout = layout
		};
		return *this;
	}


	RenderPass::Builder::Builder(const Device& device)
		: mDevice(device) {}


	RenderPass::Builder& RenderPass::Builder::WithColorAttachment(VkImageUsageFlags usage,
																  VkFormat            format,
																  VkAttachmentLoadOp  loadOp,
																  VkAttachmentStoreOp storeOp,
																  VkImageLayout       initialLayout,
																  VkImageLayout       finalLayout,
																  Core::Math::Vec4f   clearColor,
																  VkAttachmentLoadOp  stencilLoadOp, VkAttachmentStoreOp stencilStoreOp)
	{
		mAttachments.emplace_back(
			Attachment{
				.usage = usage,
				.clearColor = VkClearValue{
					.color{.float32{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}}},
				.description =
				{
					.flags = 0,
					.format = format,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = loadOp,
					.storeOp = storeOp,
					.stencilLoadOp = stencilLoadOp,
					.stencilStoreOp = stencilStoreOp,
					.initialLayout = initialLayout,
					.finalLayout = finalLayout,
				}});

		return *this;
	}


	RenderPass::Builder& RenderPass::Builder::WithDepthStencilAttachment(VkImageUsageFlags usage,
																  VkFormat            format,
																  VkAttachmentLoadOp  loadOp,
																  VkAttachmentStoreOp storeOp,
																  VkImageLayout       initialLayout,
																  VkImageLayout       finalLayout,
																  float               clearValue,
																  VkAttachmentLoadOp  stencilLoadOp,
																  VkAttachmentStoreOp stencilStoreOp)
	{
		Attachment attachment {
			.usage = usage,
			.clearColor = VkClearValue{.depthStencil{.depth = clearValue, .stencil = 0}},
			.description = VkAttachmentDescription{
				.flags = 0,
				.format = format,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = loadOp,
				.storeOp = storeOp,
				.stencilLoadOp = stencilLoadOp,
				.stencilStoreOp = stencilStoreOp,
				.initialLayout = initialLayout,
				.finalLayout = finalLayout,
		}};

		mAttachments.emplace_back(std::move(attachment));

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

		renderPass.mAttachmentFormats = mAttachments |
			std::views::transform([](auto&& x){ return x.description.format; }) |
				std::ranges::to<std::vector>();

		renderPass.mAttachmentUsages = mAttachments |
			std::views::transform([](auto&& x){ return x.usage; }) |
				std::ranges::to<std::vector>();

		renderPass.mClearColors = mAttachments |
			std::views::transform([](auto&& x) { return x.clearColor; }) |
			std::ranges::to<std::vector>();


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
				.pDepthStencilAttachment = subpass.mDepthStencilAttachment.AsPtr().UnwrapOr(nullptr),
				.preserveAttachmentCount = 0,
				.pPreserveAttachments = nullptr,
			});
		}

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		attachmentDescriptions = mAttachments |
			std::views::transform([](auto&& x) { return x.description; }) |
			std::ranges::to<std::vector>();


		VkRenderPassCreateInfo renderPassCreateInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size()),
			.pAttachments = attachmentDescriptions.data(),
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
