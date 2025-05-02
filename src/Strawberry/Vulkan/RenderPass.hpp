#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;


	class RenderPass
			: public Core::EnableReflexivePointer
	{
		friend class CommandBuffer;
		friend class GraphicsPipeline;
		friend class Framebuffer;

	public:
		class Builder;


	public:
		RenderPass(const RenderPass& rhs)            = delete;
		RenderPass& operator=(const RenderPass& rhs) = delete;
		RenderPass(RenderPass&& rhs) noexcept;
		RenderPass& operator=(RenderPass&& rhs) noexcept;
		~RenderPass();


		operator VkRenderPass() const noexcept;


		Core::ReflexivePointer<Device> GetDevice() const;

	protected:
		RenderPass(Device& device);


	private:
		VkRenderPass                   mRenderPass;
		Core::ReflexivePointer<Device> mDevice;
		std::vector<VkFormat>          mAttachmentFormats;
		std::vector<VkImageUsageFlags> mAttachmentUsages;
		std::vector<VkClearValue>      mClearColors;
	};


	class SubpassDescription
	{
		friend class RenderPass::Builder;

	public:
		SubpassDescription() = default;


		SubpassDescription& WithInputAttachment(uint32_t index, VkImageLayout layout);
		SubpassDescription& WithColorAttachment(uint32_t index, VkImageLayout layout);
		SubpassDescription& WithDepthStencilAttachment(uint32_t index, VkImageLayout layout);

	private:
		std::vector<VkAttachmentReference>    mInputAttachments;
		std::vector<VkAttachmentReference>    mColorAttachments;
		Core::Optional<VkAttachmentReference> mDepthStencilAttachment;
	};


	class RenderPass::Builder
	{
	public:
		Builder(Device& device);


		Builder& WithColorAttachment(VkImageUsageFlags usage,
									 VkFormat            format,
									 VkAttachmentLoadOp  loadOp,
									 VkAttachmentStoreOp storeOp,
									 VkImageLayout       initialLayout,
									 VkImageLayout       finalLayout,
									 Core::Math::Vec4f   clearColor     = Core::Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f),
									 VkAttachmentLoadOp  stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
		Builder& WithDepthStencilAttachment(VkImageUsageFlags usage,
									 VkFormat            format,
									 VkAttachmentLoadOp  loadOp,
									 VkAttachmentStoreOp storeOp,
									 VkImageLayout       initialLayout,
									 VkImageLayout       finalLayout,
									 float               clearValue     = 1.0f,
									 VkAttachmentLoadOp  stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);


		Builder& WithSubpass(const SubpassDescription& subpass);


		Builder WithSubpassDependency(uint32_t             srcSubpass,
		                              uint32_t             dstSubpass,
		                              VkPipelineStageFlags srcStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		                              VkPipelineStageFlags dstStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		                              VkAccessFlags        srcAccessMask = 0,
		                              VkAccessFlags        dstAccessMask = 0);


		[[nodiscard]] RenderPass Build();

	private:
		Core::ReflexivePointer<Device> mDevice;


		struct Attachment
		{
			VkImageUsageFlags usage;
			VkClearValue clearColor;
			VkAttachmentDescription description;
		};

		std::vector<Attachment> mAttachments;
		std::vector<SubpassDescription> mSubpasses;
		std::vector<VkSubpassDependency> mDependencies;
	};
} // Vulkan
