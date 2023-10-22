//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "RenderPass.hpp"
#include "Device.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	RenderPass::RenderPass(const Device& device)
		: mDevice(device)
	{
		// Render Pass
		VkAttachmentDescription attachment {
			.flags = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_GENERAL,
			.finalLayout = VK_IMAGE_LAYOUT_GENERAL,
		};
		VkAttachmentReference colorAttachment {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_GENERAL,
		};
		VkSubpassDescription subpass {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachment,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = nullptr,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr,
		};
		VkRenderPassCreateInfo renderPassCreateInfo {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = 1,
			.pAttachments = &attachment,
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 0,
			.pDependencies = nullptr,
		};
		Core::AssertEQ(vkCreateRenderPass(mDevice->mDevice, &renderPassCreateInfo, nullptr, &mRenderPass),
					   VK_SUCCESS);
	}


	RenderPass::RenderPass(RenderPass&& rhs) noexcept
		: mRenderPass(std::exchange(rhs.mRenderPass, nullptr))
		, mDevice(std::move(rhs.mDevice))
	{

	}


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
			vkDestroyRenderPass(mDevice->mDevice, mRenderPass, nullptr);
		}
	}
}
