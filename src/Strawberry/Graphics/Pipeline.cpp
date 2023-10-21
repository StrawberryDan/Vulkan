//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Pipeline.hpp"
#include "ShaderModule.hpp"
// Strawberry Graphics
#include "Strawberry/Graphics/Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <bit>
#include <utility>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Pipeline::Pipeline(Pipeline&& rhs) noexcept
		: mPipeline(std::exchange(rhs.mPipeline, nullptr))
		  , mDevice(std::exchange(rhs.mDevice, nullptr)) {}


	Pipeline& Pipeline::operator=(Pipeline&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Pipeline::~Pipeline()
	{
		if (mPipeline)
		{
			vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
			vkDestroyPipeline(mDevice, mPipeline, nullptr);
			vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
		}
	}


	Pipeline::Builder::Builder(const Device& device)
		: mDevice(&device)
		  , mStages()
	{

	}


	Pipeline::Builder& Pipeline::Builder::WithShaderStage(VkShaderStageFlagBits stage, ShaderModule shader)
	{
		Core::Assert(!mStages.contains(stage));
		Core::Assert(std::has_single_bit(std::underlying_type_t<VkShaderStageFlagBits>(stage)));
		mStages.emplace(stage, std::move(shader));
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithVertexInput(VertexInputDescription description)
	{
		mVertexInputDescription = description;
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithPrimitiveTopology(VkPrimitiveTopology topology)
	{
		mPrimitiveTopology = topology;
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithViewportSize(Core::Math::Vec2i size)
	{
		mViewportSize = size;
		return *this;
	}


	Pipeline Pipeline::Builder::Build() const
	{
		Pipeline pipeline;
		pipeline.mDevice = mDevice->mDevice;


		// Create Shader Stages
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		for (auto& [stage, shader]: mStages)
		{
			stages.emplace_back(VkPipelineShaderStageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = stage,
				.module = shader.mShaderModule,
				.pName = "main",
				.pSpecializationInfo = nullptr,
			});
		}


		// Get Vertex Input State Info
		auto bindings = mVertexInputDescription->GetBindingDescriptions();
		auto attributes = mVertexInputDescription->GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertexInputState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
			.pVertexBindingDescriptions = bindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
			.pVertexAttributeDescriptions = attributes.data()
		};


		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = mPrimitiveTopology.Value(),
			.primitiveRestartEnable = VK_FALSE,
		};


		// Tessellation State
		VkPipelineTessellationStateCreateInfo tessellationState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.patchControlPoints = 1,
		};


		// Viewport State
		VkViewport viewPort{
			.x = 0.0, .y = 0.0,
			.width = static_cast<float>(mViewportSize.Value()[0]), .height = static_cast<float>(mViewportSize.Value()[1]),
			.minDepth = 0.0,
			.maxDepth = 1.0,
		};
		VkRect2D scissorRegion{
			.offset = {0, 0},
			.extent = {static_cast<uint32_t>(mViewportSize.Value()[0]),
					   static_cast<uint32_t>(mViewportSize.Value()[1])},
		};
		VkPipelineViewportStateCreateInfo viewPortState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = &viewPort,
			.scissorCount = 1,
			.pScissors = &scissorRegion,
		};


		// Rasterization State
		VkPipelineRasterizationStateCreateInfo rasterizationState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = 0,
			.flags = 0,
			.depthClampEnable = mDepthClampEnabled,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = mPolygonMode,
			.cullMode = mCullingMode,
			.frontFace = mFrontFace,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0,
			.depthBiasClamp = 0.0,
			.depthBiasSlopeFactor = 0.0,
			.lineWidth = mLineWidth,
		};


		// Multisampling State
		VkPipelineMultisampleStateCreateInfo multisampleState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 0.0,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};


		// Depth Stencil State
		VkPipelineDepthStencilStateCreateInfo depthStencilState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = VK_FALSE,
			.depthWriteEnable = VK_FALSE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.stencilTestEnable = VK_FALSE,
			.front{},
			.back{},
			.minDepthBounds = 0.0,
			.maxDepthBounds = 1.0,
		};


		// Color Blending State
		VkPipelineColorBlendAttachmentState colorBlendAttachementState{
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
							  VK_COLOR_COMPONENT_A_BIT,
		};
		VkPipelineColorBlendStateCreateInfo colorBlendState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachementState,
			.blendConstants{0.0, 0.0, 0.0, 0.0}
		};


		// Dynamic State
		VkPipelineDynamicStateCreateInfo dynamicState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = 0,
			.pDynamicStates = nullptr,
		};


		VkPipelineLayoutCreateInfo layoutCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr,
		};
		VkPipelineLayout layout;
		Core::AssertEQ(vkCreatePipelineLayout(mDevice->mDevice, &layoutCreateInfo, nullptr, &layout), VK_SUCCESS);
		pipeline.mPipelineLayout = layout;


		// Render Pass
		VkAttachmentDescription attachment{
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
		VkSubpassDescription subpass{
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
		VkRenderPassCreateInfo renderPassCreateInfo{
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
		Core::AssertEQ(vkCreateRenderPass(mDevice->mDevice, &renderPassCreateInfo, nullptr, &pipeline.mRenderPass),
					   VK_SUCCESS);


		// Create the Pipeline
		std::vector<VkGraphicsPipelineCreateInfo> createInfos{
			VkGraphicsPipelineCreateInfo{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stageCount = static_cast<uint32_t>(stages.size()),
				.pStages = stages.data(),
				.pVertexInputState = &vertexInputState,
				.pInputAssemblyState = &inputAssembly,
				.pTessellationState = &tessellationState,
				.pViewportState = &viewPortState,
				.pRasterizationState = &rasterizationState,
				.pMultisampleState = &multisampleState,
				.pDepthStencilState = &depthStencilState,
				.pColorBlendState = &colorBlendState,
				.pDynamicState = &dynamicState,
				.layout = layout,
				.renderPass = pipeline.mRenderPass,
				.subpass = mSubpass,
				.basePipelineHandle = nullptr,
				.basePipelineIndex = 0,
			}
		};
		Core::AssertEQ(vkCreateGraphicsPipelines(mDevice->mDevice,
												 nullptr,
												 createInfos.size(),
												 createInfos.data(),
												 nullptr,
												 &pipeline.mPipeline),
					   VK_SUCCESS);

		return pipeline;
	}


	VertexInputDescription::Attribute::Attribute(uint32_t location, VkFormat format, uint32_t offset)
		: mLocation(location), mFormat(format), mOffset(offset) {}


	VertexInputDescription::Binding::Binding(uint32_t stride, VkVertexInputRate inputRate)
		: mStride(stride), mInputRate(inputRate) {}


	VertexInputDescription::Binding& VertexInputDescription::AddBinding(uint32_t stride, VkVertexInputRate inputRate)
	{
		mBindings.emplace_back(stride, inputRate);
		return mBindings.back();
	}


	const VertexInputDescription::Binding& VertexInputDescription::operator[](const uint32_t index) const
	{
		return mBindings[index];
	}


	uint32_t VertexInputDescription::Size() const
	{
		return mBindings.size();
	}


	VertexInputDescription::Binding&
	VertexInputDescription::Binding::WithAttribute(uint32_t location, VkFormat format, uint32_t offset)
	{
		mAttributes.emplace_back(location, format, offset);
		return *this;
	}


	std::vector<VkVertexInputBindingDescription> VertexInputDescription::GetBindingDescriptions() const
	{
		std::vector<VkVertexInputBindingDescription> bindings;
		for (uint32_t bindingIndex = 0; bindingIndex < Size(); bindingIndex++)
		{
			const auto& binding = mBindings[bindingIndex];
			bindings.emplace_back(VkVertexInputBindingDescription{
				.binding = bindingIndex,
				.stride = binding.mStride,
				.inputRate = binding.mInputRate
			});
		}
		return bindings;
	}


	std::vector<VkVertexInputAttributeDescription> VertexInputDescription::GetAttributeDescriptions() const
	{
		std::vector<VkVertexInputAttributeDescription> attributes;
		for (uint32_t bindingIndex = 0; bindingIndex < Size(); bindingIndex++)
		{
			const auto& binding = mBindings[bindingIndex];
			for (uint32_t attributeIndex = 0; attributeIndex < binding.mAttributes.size(); attributeIndex++)
			{
				const auto& attribute = binding.mAttributes[attributeIndex];
				attributes.emplace_back(VkVertexInputAttributeDescription{
					.location = attribute.mLocation,
					.binding = bindingIndex,
					.format = attribute.mFormat,
					.offset = attribute.mOffset
				});
			}
		}
		return attributes;
	}
}
