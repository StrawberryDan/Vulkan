//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "GraphicsPipeline.hpp"
#include "Shader.hpp"
#include "Sampler.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
// Strawberry Graphics
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <bit>
#include <cmath>
#include <utility>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& rhs) noexcept
		: mPipeline(std::exchange(rhs.mPipeline, nullptr))
		, mPipelineLayout(std::exchange(rhs.mPipelineLayout, nullptr))
		, mRenderPass(std::move(rhs.mRenderPass)) {}


	GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	GraphicsPipeline::~GraphicsPipeline()
	{
		if (mPipeline)
		{
			vkDestroyPipeline(*mRenderPass->mDevice, mPipeline, nullptr);
		}
	}


	GraphicsPipeline::Builder::Builder(PipelineLayout& layout, RenderPass& renderPass, uint32_t subpass)
		: mRenderPass(renderPass)
		, mSubpass(subpass)
		, mPipelineLayout(layout) {}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithShaderStage(VkShaderStageFlagBits stage, Shader shader)
	{
		Core::Assert(!mStages.contains(stage));
		Core::Assert(std::has_single_bit(std::underlying_type_t<VkShaderStageFlagBits>(stage)));
		mStages.emplace(stage, std::move(shader));
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithVertexInput(const std::vector<VkVertexInputBindingDescription>&   bindings,
	                                                                      const std::vector<VkVertexInputAttributeDescription>& attributes)
	{
		VkPipelineVertexInputStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
			.pVertexBindingDescriptions = bindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
			.pVertexAttributeDescriptions = attributes.data()
		};

		mVertexInputStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithInputAssembly(VkPrimitiveTopology topology)
	{
		VkPipelineInputAssemblyStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = topology,
			.primitiveRestartEnable = VK_FALSE,
		};


		mInputAssemblyStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithTesselation(uint32_t controlPoints)
	{
		VkPipelineTessellationStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.patchControlPoints = controlPoints,
		};

		mTessellationStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithViewport(const std::vector<VkViewport> viewports, const std::vector<VkRect2D> scissors)
	{
		VkPipelineViewportStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = static_cast<uint32_t>(viewports.size()),
			.pViewports = viewports.data(),
			.scissorCount = static_cast<uint32_t>(scissors.size()),
			.pScissors = scissors.data()
		};

		mViewportStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithRasterization(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace)
	{
		VkPipelineRasterizationStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = polygonMode,
			.cullMode = cullMode,
			.frontFace = frontFace,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0,
			.depthBiasClamp = 0,
			.depthBiasSlopeFactor = 0,
			.lineWidth = 1.0f,
		};

		mRasterizationStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithMultisample(VkSampleCountFlagBits samples)
	{
		VkPipelineMultisampleStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.rasterizationSamples = samples,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE,
		};

		mMultisampleStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithDepthStencil(VkPipelineDepthStencilStateCreateInfo createInfo)
	{
		mDepthStencilStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithColorBlending(std::vector<VkPipelineColorBlendAttachmentState> attachments)
	{
		VkPipelineColorBlendStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = {},
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
		};

		mColorBlendStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithDynamicState(const std::vector<VkDynamicState>& states)
	{
		VkPipelineDynamicStateCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = static_cast<uint32_t>(states.size()),
			.pDynamicStates = states.data()
		};


		mDynamicStateCreateInfo = createInfo;
		return *this;
	}


	GraphicsPipeline::GraphicsPipeline(VkPipeline handle, PipelineLayout& layout, RenderPass& renderPass)
		: mPipeline(handle)
		, mPipelineLayout(layout)
		, mRenderPass(renderPass) {}


	GraphicsPipeline GraphicsPipeline::Builder::Build()
	{
		// Input Assembly MUST be specified.
		Core::Assert(mInputAssemblyStateCreateInfo.HasValue());
		// Viewport State MUST be specified
		Core::Assert(mViewportStateCreateInfo.HasValue());
		// Rasterization State MUST be specified.
		Core::Assert(mRasterizationStateCreateInfo.HasValue());
		// Color Blend State MUST be specified
		Core::Assert(mColorBlendStateCreateInfo.HasValue());


		VkSpecializationInfo specializationInfo
		{
			.mapEntryCount = static_cast<uint32_t>(mShaderSpecializationEntries.size()),
			.pMapEntries = mShaderSpecializationEntries.data(),
			.dataSize = mShaderSpecializationData.Size(),
			.pData = mShaderSpecializationData.Data()
		};


		// Create Shader Stages
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		for (auto& [stage, shader]: mStages)
		{
			stages.emplace_back(VkPipelineShaderStageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = stage,
				.module = shader,
				.pName = "main",
				.pSpecializationInfo = mShaderSpecializationEntries.empty() ? nullptr : &specializationInfo,
			});
		}


		// Use default vertex input if none specified
		if (!mVertexInputStateCreateInfo)
		{
			mVertexInputStateCreateInfo = VkPipelineVertexInputStateCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.vertexBindingDescriptionCount = 0,
				.pVertexBindingDescriptions = nullptr,
				.vertexAttributeDescriptionCount = 0,
				.pVertexAttributeDescriptions = nullptr
			};
		}


		// Use default multisampling if now specified.
		if (!mMultisampleStateCreateInfo)
		{
			mMultisampleStateCreateInfo = VkPipelineMultisampleStateCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
				.minSampleShading = 1.0,
				.pSampleMask = nullptr,
				.alphaToCoverageEnable = VK_FALSE,
				.alphaToOneEnable = VK_FALSE,
			};
		}


		// Create the Pipeline
		VkGraphicsPipelineCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stageCount = static_cast<uint32_t>(stages.size()),
			.pStages = stages.data(),
			.pVertexInputState = mVertexInputStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pInputAssemblyState = mInputAssemblyStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pTessellationState = mTessellationStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pViewportState = mViewportStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pRasterizationState = mRasterizationStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pMultisampleState = mMultisampleStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pDepthStencilState = mDepthStencilStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pColorBlendState = mColorBlendStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pDynamicState = mDynamicStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.layout = *mPipelineLayout,
			.renderPass = mRenderPass->mRenderPass,
			.subpass = mSubpass,
			.basePipelineHandle = nullptr,
			.basePipelineIndex = 0,
		};


		VkPipeline handle = VK_NULL_HANDLE;
		Core::AssertEQ(vkCreateGraphicsPipelines(*mRenderPass->mDevice,
		                                         nullptr,
		                                         1,
		                                         &createInfo,
		                                         nullptr,
		                                         &handle),
		               VK_SUCCESS);

		return GraphicsPipeline(handle, *mPipelineLayout, *mRenderPass);
	}
}
