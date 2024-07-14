//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "PipelineLayout.hpp"

#include <utility>


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	PipelineLayout::PipelineLayout(VkPipelineLayout handle, Core::ReflexivePointer<Device> device, std::vector<VkDescriptorSetLayout> setLayouts)
		: mHandle(handle)
		, mDevice(std::move(device))
		, mSetLayouts(std::move(setLayouts)) {}


	PipelineLayout::PipelineLayout(PipelineLayout&& rhs)
		: mHandle(std::exchange(rhs.mHandle, VK_NULL_HANDLE))
		, mDevice(std::move(rhs.mDevice))
		, mSetLayouts(std::move(rhs.mSetLayouts)) {}


	PipelineLayout& PipelineLayout::operator=(PipelineLayout&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	PipelineLayout::~PipelineLayout()
	{
		for (auto setLayout: mSetLayouts)
		{
			vkDestroyDescriptorSetLayout(*mDevice, setLayout, nullptr);
		}

		if (mHandle)
		{
			vkDestroyPipelineLayout(*mDevice, mHandle, nullptr);
		}
	}


	PipelineLayout::operator VkPipelineLayout() const
	{
		return mHandle;
	}


	VkDescriptorSetLayout PipelineLayout::GetSetLayout(uint32_t index)
	{
		return mSetLayouts[index];
	}


	PipelineLayout::Builder::Builder(const Device& device)
		: mDevice(device) {}


	PipelineLayout::Builder::Builder(Builder&& rhs)
		: mDevice(std::move(rhs.mDevice))
		, mSetLayouts(std::move(rhs.mSetLayouts))
		, mPushConstantRanges(std::move(rhs.mPushConstantRanges)) {}


	PipelineLayout::Builder& PipelineLayout::Builder::operator=(Builder&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	PipelineLayout::Builder::~Builder()
	{
		for (auto layout: mSetLayouts)
		{
			vkDestroyDescriptorSetLayout(*mDevice, layout, nullptr);
		}
	}


	PipelineLayout::Builder& PipelineLayout::Builder::WithDescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		VkDescriptorSetLayoutCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = bindings.data()
		};

		VkDescriptorSetLayout handle = VK_NULL_HANDLE;
		vkCreateDescriptorSetLayout(*mDevice, &createInfo, nullptr, &handle);
		mSetLayouts.emplace_back(handle);

		return *this;
	}


	PipelineLayout::Builder& PipelineLayout::Builder::WithPushConstantRange(uint32_t size, uint32_t offset, VkShaderStageFlags stageFlags)
	{
		VkPushConstantRange range{
			.stageFlags = stageFlags,
			.offset = offset,
			.size = size,
		};

		mPushConstantRanges.emplace_back(range);

		return *this;
	}


	PipelineLayout PipelineLayout::Builder::Build()
	{
		VkPipelineLayoutCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = static_cast<uint32_t>(mSetLayouts.size()),
			.pSetLayouts = mSetLayouts.data(),
			.pushConstantRangeCount = static_cast<uint32_t>(mPushConstantRanges.size()),
			.pPushConstantRanges = mPushConstantRanges.data()
		};


		VkPipelineLayout handle = VK_NULL_HANDLE;
		vkCreatePipelineLayout(*mDevice, &createInfo, nullptr, &handle);


		return PipelineLayout(handle, mDevice, std::move(mSetLayouts));
	}
}
