#include "ComputePipeline.hpp"


namespace Strawberry::Vulkan
{
	ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
		: mPipelineLayout(std::move(other.mPipelineLayout))
		, mPipeline(std::exchange(other.mPipeline, VK_NULL_HANDLE))
	{}

	ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
	{
		if (this != &other)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}

	ComputePipeline::~ComputePipeline() noexcept
	{
		if (mPipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(mDevice, mPipeline, nullptr);
		}
	}

	ComputePipeline::operator struct VkPipeline_T*() const
	{
		return mPipeline;
	}

	PipelineLayout& ComputePipeline::GetLayout()
	{
		return *mPipelineLayout;
	}

	const PipelineLayout& ComputePipeline::GetLayout() const
	{
		return *mPipelineLayout;
	}

	ComputePipeline::ComputePipeline(const Device& device, PipelineLayout& layout, VkPipeline&& pipeline)
		: mDevice(device)
		, mPipelineLayout(layout)
		, mPipeline(std::exchange(pipeline, VK_NULL_HANDLE))
	{}

	ComputePipeline::Builder::Builder(const Device& device, PipelineLayout& layout, Shader&& shader)
		: mDevice(device)
		, mPipelineLayout(layout)
		, mShader(std::move(shader))
	{}


	ComputePipeline ComputePipeline::Builder::Build()
	{
		VkSpecializationInfo specializationInfo
		{
			.mapEntryCount = static_cast<uint32_t>(mShaderSpecializationEntries.size()),
			.pMapEntries = mShaderSpecializationEntries.data(),
			.dataSize = mShaderSpecializationData.Size(),
			.pData = mShaderSpecializationData.Data()
		};


		VkPipelineShaderStageCreateInfo stageCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = mShader,
			.pName = "main",
			.pSpecializationInfo = &specializationInfo,
		};


		VkComputePipelineCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = stageCreateInfo,
			.layout = mPipelineLayout,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = 0
		};

		VkPipeline pipeline = VK_NULL_HANDLE;
		Core::AssertEQ(vkCreateComputePipelines(
			mDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline), VK_SUCCESS);


		return ComputePipeline(mDevice, mPipelineLayout, std::move(pipeline));
	}
}
