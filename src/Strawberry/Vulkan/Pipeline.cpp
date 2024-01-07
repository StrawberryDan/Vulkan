//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Pipeline.hpp"
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
	Pipeline::Pipeline(Pipeline&& rhs) noexcept
		: mPipeline(std::exchange(rhs.mPipeline, nullptr))
		  , mRenderPass(std::move(rhs.mRenderPass))
		  , mPipelineLayout(std::exchange(rhs.mPipelineLayout, nullptr))
		  , mViewport(std::exchange(rhs.mViewport, {}))
		  , mDescriptorPool(std::move(rhs.mDescriptorPool))
		  , mDescriptorSetLayouts(std::move(rhs.mDescriptorSetLayouts))
	{

	}


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
			vkDestroyPipelineLayout(mRenderPass->mDevice->mDevice, mPipelineLayout, nullptr);
			for (VkDescriptorSetLayout layout: mDescriptorSetLayouts)
				vkDestroyDescriptorSetLayout(mRenderPass->mDevice->mDevice, layout, nullptr);
			vkDestroyPipeline(mRenderPass->mDevice->mDevice, mPipeline, nullptr);
		}
	}


	Pipeline::Builder::Builder(const RenderPass& renderPass, uint32_t subpass)
		: mRenderPass(renderPass)
		, mSubpass(subpass)
	{

	}


	Pipeline::Builder& Pipeline::Builder::WithShaderStage(VkShaderStageFlagBits stage, Shader shader)
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


	Pipeline::Builder& Pipeline::Builder::WithViewport(Core::Math::Vec2f offset, Core::Math::Vec2f size)
	{
		mViewport = VkViewport {
			.x = offset[0],
			.y = offset[1],
			.width = size[0],
			.height = size[1],
			.maxDepth = 1.0,
		};
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithDepthClamping()
	{
		mDepthClampEnabled = VK_TRUE;
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithDepthTesting()
	{
		mDepthTestingEnabled = true;
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithColorBlending()
	{
		mColorBlendingEnabled = true;
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithCullMode(VkCullModeFlags cullModeFlags)
	{
		mCullingMode = cullModeFlags;
		return *this;
	}


	Pipeline::Builder&
	Pipeline::Builder::WithPushConstantRange(VkShaderStageFlags stage, uint32_t size, uint32_t offset)
	{
		mPushConstantRanges.emplace_back(VkPushConstantRange {
			.stageFlags = stage,
			.offset = offset,
			.size = size,
		});
		return *this;
	}


	Pipeline::Builder& Pipeline::Builder::WithDescriptorSetLayout(const DescriptorSetLayout& descriptorSetLayout)
	{
		VkDescriptorSetLayout layout;
		VkDescriptorSetLayoutCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(descriptorSetLayout.mBindings.size()),
			.pBindings = descriptorSetLayout.mBindings.data()
		};
		Core::AssertEQ(vkCreateDescriptorSetLayout(mRenderPass->mDevice->mDevice, &createInfo, nullptr, &layout), VK_SUCCESS);
		mDescriptorSetLayouts.emplace_back(layout);


		for (auto layoutBinding: descriptorSetLayout.mBindings)
		{
			mDescriptorPoolSizes.emplace_back(VkDescriptorPoolSize {
				.type = layoutBinding.descriptorType,
				.descriptorCount = layoutBinding.descriptorCount
			});
		}


		return *this;
	}


	DescriptorSet Pipeline::AllocateDescriptorSet(size_t layoutIndex) const
	{
		return mDescriptorPool->Create<DescriptorSet>(mDescriptorSetLayouts[layoutIndex]);
	}


	Pipeline::Pipeline() = default;


	Pipeline Pipeline::Builder::Build() const
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
				.module = shader.mShaderModule,
				.pName = "main",
				.pSpecializationInfo = nullptr,
			});
		}


		// Get Vertex Input State Info
		auto bindings = mVertexInputDescription.GetBindingDescriptions();
		auto attributes = mVertexInputDescription.GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertexInputState {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
			.pVertexBindingDescriptions = bindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
			.pVertexAttributeDescriptions = attributes.data()
		};


		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = mPrimitiveTopology.Value(),
			.primitiveRestartEnable = VK_FALSE,
		};


		// Tessellation State
		VkPipelineTessellationStateCreateInfo tessellationState {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.patchControlPoints = 1,
		};


		// Viewport State
		VkRect2D scissorRegion {
			.offset = {0, 0},
			.extent = {static_cast<uint32_t>(std::ceil(mViewport->width)),
					   static_cast<uint32_t>(std::ceil(mViewport->height))},
		};
		VkPipelineViewportStateCreateInfo viewPortState {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = mViewport.AsPtr().Unwrap(),
			.scissorCount = 1,
			.pScissors = &scissorRegion,
		};


		// Rasterization State
		VkPipelineRasterizationStateCreateInfo rasterizationState {
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
		VkPipelineMultisampleStateCreateInfo multisampleState {
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
		VkPipelineDepthStencilStateCreateInfo depthStencilState {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = mDepthTestingEnabled ? VK_TRUE : VK_FALSE,
			.depthWriteEnable = mDepthTestingEnabled ? VK_TRUE : VK_FALSE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.stencilTestEnable = VK_FALSE,
			.front{},
			.back{},
			.minDepthBounds = 0.0,
			.maxDepthBounds = 1.0,
		};


		// Color Blending State
		VkPipelineColorBlendAttachmentState colorBlendAttachementState {
			.blendEnable = mColorBlendingEnabled ? VK_TRUE : VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
							  VK_COLOR_COMPONENT_A_BIT,
		};

		VkPipelineColorBlendStateCreateInfo colorBlendState {
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
		VkPipelineDynamicStateCreateInfo dynamicState {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = 0,
			.pDynamicStates = nullptr,
		};


		// Create Descriptor pool and actual Pipeline Object
		Pipeline pipeline;
		pipeline.mRenderPass = mRenderPass;
		pipeline.mViewport = mViewport.Value();
		pipeline.mDescriptorSetLayouts = mDescriptorSetLayouts;


		// Pipeline layout
		VkPipelineLayoutCreateInfo layoutCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size()),
			.pSetLayouts = mDescriptorSetLayouts.data(),
			.pushConstantRangeCount = static_cast<uint32_t>(mPushConstantRanges.size()),
			.pPushConstantRanges = mPushConstantRanges.data(),
		};
		Core::AssertEQ(vkCreatePipelineLayout(mRenderPass->mDevice->mDevice, &layoutCreateInfo, nullptr, &pipeline.mPipelineLayout), VK_SUCCESS);


		// Create the Pipeline
		std::vector<VkGraphicsPipelineCreateInfo> createInfos {
			VkGraphicsPipelineCreateInfo {
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
				.layout = pipeline.mPipelineLayout,
				.renderPass = mRenderPass->mRenderPass,
				.subpass = mSubpass,
				.basePipelineHandle = nullptr,
				.basePipelineIndex = 0,
			}
		};
		Core::AssertEQ(vkCreateGraphicsPipelines(mRenderPass->mDevice->mDevice,
												 nullptr,
												 createInfos.size(),
												 createInfos.data(),
												 nullptr,
												 &pipeline.mPipeline),
					   VK_SUCCESS);

		if (!mDescriptorSetLayouts.empty())
		{
			DescriptorPool descriptorPool(*mRenderPass->GetDevice(), 0, mDescriptorSetLayouts.size(), mDescriptorPoolSizes);
			pipeline.mDescriptorPool = std::move(descriptorPool);
		}

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
			bindings.emplace_back(VkVertexInputBindingDescription {
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
				attributes.emplace_back(VkVertexInputAttributeDescription {
					.location = attribute.mLocation,
					.binding = bindingIndex,
					.format = attribute.mFormat,
					.offset = attribute.mOffset
				});
			}
		}
		return attributes;
	}


	DescriptorSetLayout&
	DescriptorSetLayout::WithBinding(VkDescriptorType type, uint32_t count, VkShaderStageFlags stage)
	{
		mBindings.emplace_back(VkDescriptorSetLayoutBinding {
			.binding = static_cast<uint32_t>(mBindings.size()),
			.descriptorType = type,
			.descriptorCount = count,
			.stageFlags = stage,
			.pImmutableSamplers = nullptr,
		});
		return *this;
	}
}
