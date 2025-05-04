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
		GraphicsPipeline(const GraphicsPipeline& rhs)            = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline& rhs) = delete;
		GraphicsPipeline(GraphicsPipeline&& rhs) noexcept;
		GraphicsPipeline& operator=(GraphicsPipeline&& rhs) noexcept;
		~GraphicsPipeline();


		Core::ReflexivePointer<PipelineLayout> GetLayout() const noexcept { return mPipelineLayout; }


		template<std::movable T, typename... Args>
		T Create(const Args&... args) const
		{
			return T(*this, std::forward<const Args&>(args)...);
		}


	private:
		GraphicsPipeline(VkPipeline handle, PipelineLayout& layout, RenderPass& renderPass);


	private:
		// Handle to pipeline
		VkPipeline mPipeline = VK_NULL_HANDLE;
		// Our Layout
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;
		// Our RenderPass
		Core::ReflexivePointer<RenderPass> mRenderPass;
	};


	class GraphicsPipeline::Builder
	{
	public:
		explicit Builder(PipelineLayout& layout, RenderPass& renderPass, uint32_t subpass = 0);


		Builder& WithShaderStage(VkShaderStageFlagBits stage, Shader shader);


		Builder& WithInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
		Builder& WithInputAttribute(uint32_t location, uint32_t binding, uint32_t offset, VkFormat format);
		Builder& WithInputAssembly(VkPrimitiveTopology topology);
		Builder& WithTesselation(uint32_t controlPoints);
		Builder& WithViewport(const std::vector<VkViewport>& viewports, const std::vector<VkRect2D>& scissors);
		// Set the viewport and scissor regions to cover an entire frame buffer.
		Builder& WithViewport(const Framebuffer& framebuffer);
		Builder& WithRasterization(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
		Builder& WithMultisample(VkSampleCountFlagBits samples);
		Builder& WithDepthStencil(VkPipelineDepthStencilStateCreateInfo createInfo);
		Builder& WithDepthTesting();
		Builder& WithColorBlending(std::vector<VkPipelineColorBlendAttachmentState> attachments);
		Builder& WithColorBlending(const VkPipelineColorBlendAttachmentState& attachment);
		Builder& WithAlphaColorBlending();
		Builder& WithDynamicState(const std::vector<VkDynamicState>& states);


		template <typename T, typename... Ts>
		Builder& WithShaderSpecializationConstants(T t, Ts... ts)
		{
			VkSpecializationMapEntry entry
			{
				.constantID = static_cast<uint32_t>(mShaderSpecializationEntries.size()),
				.offset = static_cast<uint32_t>(mShaderSpecializationData.Size()),
				.size = sizeof(T),
			};
			mShaderSpecializationEntries.emplace_back(entry);

			mShaderSpecializationData.Push(std::forward<T>(t));


			if constexpr(sizeof...(ts) > 0)
			{
				return WithShaderSpecializationConstants(std::forward<Ts>(ts)...);
			}

			return *this;
		}


		[[nodiscard]] GraphicsPipeline Build();

	private:
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;
		Core::ReflexivePointer<RenderPass>     mRenderPass;
		uint32_t                               mSubpass;


		std::map<VkShaderStageFlagBits, Shader> mStages;
		std::vector<VkSpecializationMapEntry> mShaderSpecializationEntries;
		Core::IO::DynamicByteBuffer mShaderSpecializationData;


		Core::Optional<VkPipelineInputAssemblyStateCreateInfo> mInputAssemblyStateCreateInfo;
		Core::Optional<VkPipelineTessellationStateCreateInfo>  mTessellationStateCreateInfo;
		Core::Optional<VkPipelineRasterizationStateCreateInfo> mRasterizationStateCreateInfo;
		Core::Optional<VkPipelineMultisampleStateCreateInfo>   mMultisampleStateCreateInfo;
		Core::Optional<VkPipelineDepthStencilStateCreateInfo>  mDepthStencilStateCreateInfo;
		Core::Optional<VkPipelineDynamicStateCreateInfo>       mDynamicStateCreateInfo;


		std::vector<VkVertexInputBindingDescription>     mVertexInputBindings;
		std::vector<VkVertexInputAttributeDescription>   mVertexInputAttributes;
		std::vector<VkPipelineColorBlendAttachmentState> mColorBlendingAttachmentStates;
		std::vector<VkViewport>                          mViewports;
		std::vector<VkRect2D>                            mScissorRegions;
	};
}
