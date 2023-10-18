//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Pipeline.hpp"
// Strawberry Graphics
#include "Strawberry/Graphics/Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <vector>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Pipeline::Pipeline(Pipeline&& rhs)
		: mPipeline(std::exchange(rhs.mPipeline, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
	{}


	Pipeline& Pipeline::operator=(Pipeline&& rhs)
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
			vkDestroyPipeline(mDevice, mPipeline, nullptr);
		}
	}


	Pipeline::Builder::Builder(const Device& device)
		: mDevice(&device)
	{

	}


	Pipeline Pipeline::Builder::Build() const
	{
		Pipeline pipeline;
		pipeline.mDevice = mDevice->mDevice;

		std::vector<VkGraphicsPipelineCreateInfo> createInfos;

		Core::AssertEQ(vkCreateGraphicsPipelines(mDevice->mDevice, nullptr, createInfos.size(), createInfos.data(), nullptr, &pipeline.mPipeline), VK_SUCCESS);

		return pipeline;
	}
}
