#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Shader.hpp"
#include "Buffer.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSet.hpp"
#include "PipelineLayout.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <map>
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;
	class Sampler;
	class ImageView;
	class RenderPass;


	class GraphicsPipeline
	{
		friend class Builder;
		friend class CommandBuffer;
		friend class Framebuffer;


	public:
		class Builder;


	public:
		GraphicsPipeline(const GraphicsPipeline& rhs) = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline& rhs) = delete;
		GraphicsPipeline(GraphicsPipeline&& rhs) noexcept;
		GraphicsPipeline& operator=(GraphicsPipeline&& rhs) noexcept;
		~GraphicsPipeline();


		template <std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


	private:
		GraphicsPipeline(VkPipeline handle, PipelineLayout& layout, RenderPass& renderPass);


	private:
		// Handle to pipeline
		VkPipeline mPipeline = nullptr;
		// Our Layout
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;
		// Our RenderPass
		Core::ReflexivePointer<RenderPass> mRenderPass;
	};

	class GraphicsPipeline::Builder
	{
	public:
		explicit Builder(const PipelineLayout& layout, const RenderPass& renderPass, uint32_t subpass = 0);


		Builder& WithShaderStage(VkShaderStageFlagBits stage, Shader shader);


		Builder& WithVertexInput(const std::vector<VkVertexInputBindingDescription>& bindings, const std::vector<VkVertexInputAttributeDescription>& attributes);
		Builder& WithInputAssembly(VkPrimitiveTopology topology, bool primitiveRestart);
		Builder& WithTesselation(uint32_t controlPoints);
		Builder& WithViewport(const std::vector<VkViewport> viewports, const std::vector<VkRect2D> scissors);
		Builder& WithRasterization(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
		Builder& WithMultisample(VkSampleCountFlagBits samples);
		Builder& WithDepthStencil(VkPipelineDepthStencilStateCreateInfo createInfo);
		Builder& WithColorBlending(std::vector<VkPipelineColorBlendAttachmentState> attachments);
		Builder& WithDynamicState(const std::vector<VkDynamicState>& states);


		[[nodiscard]] GraphicsPipeline Build();


	private:
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;
		Core::ReflexivePointer<RenderPass> mRenderPass;
		uint32_t mSubpass;


		std::map<VkShaderStageFlagBits, Shader> mStages;


		Core::Optional<VkPipelineVertexInputStateCreateInfo> mVertexInputStateCreateInfo;
		Core::Optional<VkPipelineInputAssemblyStateCreateInfo> mInputAssemblyStateCreateInfo;
		Core::Optional<VkPipelineTessellationStateCreateInfo> mTessellationStateCreateInfo;
		Core::Optional<VkPipelineViewportStateCreateInfo> mViewportStateCreateInfo;
		Core::Optional<VkPipelineRasterizationStateCreateInfo> mRasterizationStateCreateInfo;
		Core::Optional<VkPipelineMultisampleStateCreateInfo> mMultisampleStateCreateInfo;
		Core::Optional<VkPipelineDepthStencilStateCreateInfo> mDepthStencilStateCreateInfo;
		Core::Optional<VkPipelineColorBlendStateCreateInfo> mColorBlendStateCreateInfo;
		Core::Optional<VkPipelineDynamicStateCreateInfo> mDynamicStateCreateInfo;
	};
}
