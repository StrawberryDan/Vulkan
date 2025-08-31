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

#include "Framebuffer.hpp"


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

	GraphicsPipeline::operator struct VkPipeline_T*() const
	{
		return mPipeline;
	}

	const PipelineLayout& GraphicsPipeline::GetLayout() const noexcept
	{
		return *mPipelineLayout;
	}


	GraphicsPipeline::Builder::Builder(PipelineLayout& layout, RenderPass& renderPass, uint32_t subpass)
		: mRenderPass(renderPass)
		  , mSubpass(subpass)
		  , mPipelineLayout(layout)
	{
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithShaderStage(VkShaderStageFlagBits stage, Shader shader)
	{
		Core::Assert(!mStages.contains(stage));
		Core::Assert(std::has_single_bit(std::underlying_type_t<VkShaderStageFlagBits>(stage)));
		mStages.emplace(stage, std::move(shader));
		return *this;
	}

	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithInputBinding(uint32_t binding, uint32_t stride,
		VkVertexInputRate inputRate)
	{
		mVertexInputBindings.emplace_back(VkVertexInputBindingDescription{
			.binding = binding, .stride = stride, .inputRate = inputRate
		});
		return *this;
	}

	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithInputAttribute(uint32_t location, uint32_t binding,
		uint32_t offset, VkFormat format)
	{
		mVertexInputAttributes.emplace_back(VkVertexInputAttributeDescription{
			.location = location, .binding = binding, .format = format, .offset = offset
		});
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


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithViewport(const std::vector<VkViewport>& viewports,
																	   const std::vector<VkRect2D>& scissors)
	{
		mViewports.append_range(viewports);
		mScissorRegions.append_range(scissors);
		return *this;
	}

	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithViewport(const Framebuffer& framebuffer, bool negateY)
	{
		return WithViewport(
			{
				VkViewport{
					.x = 0,
					.y = negateY ? static_cast<float>(framebuffer.GetSize()[1]) : 0,
					.width = framebuffer.GetSize().AsType<float>()[0],
					.height = negateY ? -static_cast<float>(framebuffer.GetSize()[1]) : static_cast<float>(framebuffer.GetSize()[1]),
					.minDepth = 0.0,
					.maxDepth = 1.0
				}
			},
			{
				VkRect2D{
					.offset = VkOffset2D
					{
						.x = 0,
						.y = 0
					},
					.extent = VkExtent2D
					{
						.width = framebuffer.GetSize()[0],
						.height = framebuffer.GetSize()[1]
					}
				}
			}
		);
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithRasterization(
		VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace)
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


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithDepthStencil(
		VkPipelineDepthStencilStateCreateInfo createInfo)
	{
		mDepthStencilStateCreateInfo = createInfo;
		return *this;
	}

	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithDepthTesting()
	{
		return WithDepthStencil({
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
		});
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithColorBlending(
		std::vector<VkPipelineColorBlendAttachmentState> attachments)
	{
		mColorBlendingAttachmentStates.append_range(attachments);
		return *this;
	}


	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithColorBlending(
		const VkPipelineColorBlendAttachmentState& attachment)
	{
		mColorBlendingAttachmentStates.emplace_back(attachment);
		return *this;
	}

	GraphicsPipeline::Builder& GraphicsPipeline::Builder::WithAlphaColorBlending()
	{
		mColorBlendingAttachmentStates.emplace_back(
			VkPipelineColorBlendAttachmentState{
				.blendEnable = VK_TRUE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
				.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				.colorBlendOp = VK_BLEND_OP_ADD,
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
				.alphaBlendOp = VK_BLEND_OP_ADD,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			}
		);
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


	Device& GraphicsPipeline::GetDevice() noexcept
	{
		return mRenderPass->GetDevice();
	}


	const Device& GraphicsPipeline::GetDevice() const noexcept
	{
		return mRenderPass->GetDevice();
	}

	GraphicsPipeline::GraphicsPipeline(VkPipeline handle, PipelineLayout& layout, RenderPass& renderPass)
		: mPipeline(handle)
		  , mPipelineLayout(layout)
		  , mRenderPass(renderPass)
	{
	}


	GraphicsPipeline GraphicsPipeline::Builder::Build()
	{
		// Input Assembly MUST be specified.
		Core::Assert(mInputAssemblyStateCreateInfo.HasValue());
		// At least one viewport and once scissor region must be specified
		Core::Assert(!mViewports.empty());
		Core::Assert(!mScissorRegions.empty());
		// Rasterization State MUST be specified.
		Core::Assert(mRasterizationStateCreateInfo.HasValue());


		VkSpecializationInfo specializationInfo
		{
			.mapEntryCount = static_cast<uint32_t>(mShaderSpecializationEntries.size()),
			.pMapEntries = mShaderSpecializationEntries.data(),
			.dataSize = mShaderSpecializationData.Size(),
			.pData = mShaderSpecializationData.Data()
		};


		// Create Shader Stages
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		for (auto& [stage, shader] : mStages)
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


		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = {},
			.attachmentCount = static_cast<uint32_t>(mColorBlendingAttachmentStates.size()),
			.pAttachments = mColorBlendingAttachmentStates.data(),
			.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
		};


		VkPipelineViewportStateCreateInfo viewportStateCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = static_cast<uint32_t>(mViewports.size()),
			.pViewports = mViewports.data(),
			.scissorCount = static_cast<uint32_t>(mScissorRegions.size()),
			.pScissors = mScissorRegions.data()
		};


		VkPipelineVertexInputStateCreateInfo inputStateCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = static_cast<uint32_t>(mVertexInputBindings.size()),
			.pVertexBindingDescriptions = mVertexInputBindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(mVertexInputAttributes.size()),
			.pVertexAttributeDescriptions = mVertexInputAttributes.data()
		};


		// Create the Pipeline
		VkGraphicsPipelineCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stageCount = static_cast<uint32_t>(stages.size()),
			.pStages = stages.data(),
			.pVertexInputState = &inputStateCreateInfo,
			.pInputAssemblyState = mInputAssemblyStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pTessellationState = mTessellationStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pViewportState = &viewportStateCreateInfo,
			.pRasterizationState = mRasterizationStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pMultisampleState = mMultisampleStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pDepthStencilState = mDepthStencilStateCreateInfo.AsPtr().UnwrapOr(nullptr),
			.pColorBlendState = &colorBlendCreateInfo,
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
