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
		, mRenderPass(std::move(rhs.mRenderPass))
	{

	}


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


	GraphicsPipeline::Builder::Builder(const PipelineLayout& layout, const RenderPass& renderPass, uint32_t subpass)
		: mRenderPass(renderPass)
		, mSubpass(subpass)
		, mPipelineLayout(layout)
	{}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithShaderStage(VkShaderStageFlagBits stage, Shader shader)
	{
		Core::Assert(!mStages.contains(stage));
		Core::Assert(std::has_single_bit(std::underlying_type_t<VkShaderStageFlagBits>(stage)));
		mStages.emplace(stage, std::move(shader));
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithVertexInput()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithInputAssembly()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithTesselation()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithViewport()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithRasterization()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithMultisample()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithDepthStencil()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithColorBlending()
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithDynamicState()
	{
	}


	GraphicsPipeline::GraphicsPipeline(VkPipeline handle, PipelineLayout& layout, RenderPass& renderPass)
		: mPipeline(handle)
		, mPipelineLayout(layout)
		, mRenderPass(renderPass)
	{

	}


	GraphicsPipeline GraphicsPipeline::Builder::Build() const
	{
		// Create Shader Stages
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		for (auto& [stage, shader]: mStages)
		{
			stages.emplace_back(VkPipelineShaderStageCreateInfo {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = stage,
				.module = shader,
				.pName = "main",
				.pSpecializationInfo = nullptr,
			});
		}


		// Create the Pipeline
		std::vector<VkGraphicsPipelineCreateInfo> createInfos {
			VkGraphicsPipelineCreateInfo {
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
			}
		};
		VkPipeline handle = VK_NULL_HANDLE;
		Core::AssertEQ(vkCreateGraphicsPipelines(*mRenderPass->mDevice,
												 nullptr,
												 createInfos.size(),
												 createInfos.data(),
												 nullptr,
												 &handle),
					   VK_SUCCESS);

		return GraphicsPipeline(handle, *mPipelineLayout, *mRenderPass);
	}
}
