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
		: public Core::EnableReflexivePointer<RenderPass>
	{
		friend class CommandBuffer;
		friend class GraphicsPipeline;
		friend class Framebuffer;


	public:
		class Builder;


	public:
		RenderPass(const RenderPass& rhs) = delete;
		RenderPass& operator=(const RenderPass& rhs) = delete;
		RenderPass(RenderPass&& rhs) noexcept;
		RenderPass& operator=(RenderPass&& rhs) noexcept;
		~RenderPass();


		template<std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


		Core::ReflexivePointer<Device> GetDevice() const;


	protected:
		RenderPass(const Device& device);


	private:
		VkRenderPass mRenderPass;
		Core::ReflexivePointer<Device> mDevice;
		std::vector<VkFormat> mColorAttachmentFormats;
		std::vector<VkClearValue> mClearColors;
	};


	class SubpassDescription
	{
		friend class RenderPass::Builder;


	public:
		SubpassDescription() = default;


		SubpassDescription& WithInputAttachment(uint32_t index);
		SubpassDescription& WithColorAttachment(uint32_t index);
		SubpassDescription& WithDepthStencilAttachment(uint32_t index);


	private:
		std::vector<VkAttachmentReference> mInputAttachments;
		std::vector<VkAttachmentReference> mColorAttachments;
		Core::Optional<VkAttachmentReference> mDepthStencilAttachment;
	};


	class RenderPass::Builder
	{
	public:
		Builder(const Device& device);


		Builder& WithColorAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
									 Core::Math::Vec4f clearColor = Core::Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f),
									 VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
									 VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
		Builder& WithDepthAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
		                             float clearValue = 0.0f,
		                             VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		                             VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
		Builder& WithStencilAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
		                               uint32_t clearValue = 0,
		                               VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		                               VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);


		Builder& WithSubpass(const SubpassDescription& subpass);


		Builder WithSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass,
									  VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									  VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
									  VkAccessFlags srcAccessMask = 0, VkAccessFlags dstAccessMask = 0);


		[[nodiscard]] RenderPass Build();


	private:
		Core::ReflexivePointer<Device> mDevice;

		std::vector<VkAttachmentDescription> mAttachments;
		std::vector<SubpassDescription> mSubpasses;
		std::vector<VkSubpassDependency> mDependencies;
		std::vector<VkClearValue> mClearColors;
	};
} // Vulkan
