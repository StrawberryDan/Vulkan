//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Pipeline/PipelineLayout.hpp"
#include "Strawberry/Vulkan/Device/Device.hpp"
// Standary Library
#include <utility>


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	PipelineLayout::PipelineLayout(VkPipelineLayout                 handle,
								   Device&                          device,
								   std::vector<DescriptorSetLayout> setLayouts)
		: mHandle(handle)
		  , mDevice(device)
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
			vkDestroyDescriptorSetLayout(mDevice->Handle(), setLayout.Handle(), nullptr);
		}

		if (mHandle)
		{
			vkDestroyPipelineLayout(mDevice->Handle(), mHandle, nullptr);
		}
	}


	Core::ReflexivePointer<Device> PipelineLayout::GetDevice() const
	{
		return mDevice;
	}


	VkPipelineLayout PipelineLayout::Handle() const
	{
		return mHandle;
	}


	PipelineLayout::operator VkPipelineLayout() const
	{
		return mHandle;
	}


	DescriptorSetLayout PipelineLayout::GetSetLayout(uint32_t index)
	{
		Core::Assert(index < mSetLayouts.size());
		return mSetLayouts[index];
	}


	PipelineLayout::Builder::Builder(Device& device)
		: mDevice(device) {}


	PipelineLayout::Builder::Builder(Builder&& rhs)
		: mDevice(std::move(rhs.mDevice))
		  , mBindings(std::move(rhs.mBindings))
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


	PipelineLayout::Builder& PipelineLayout::Builder::WithDescriptor(
		unsigned int       set,
		VkDescriptorType   type,
		VkShaderStageFlags shaderStages,
		unsigned int       count)
	{
		if (!mBindings.contains(set))
		{
			mBindings.insert_or_assign(set, std::vector<VkDescriptorSetLayoutBinding>());
		}

		VkDescriptorSetLayoutBinding binding {
			.binding = static_cast<uint32_t>(mBindings[set].size()),
			.descriptorType = type,
			.descriptorCount = count,
			.stageFlags = shaderStages,
			.pImmutableSamplers = nullptr
		};

		mBindings.at(set).emplace_back(binding);

		return *this;
	}


	PipelineLayout::Builder& PipelineLayout::Builder::WithPushConstantRange(
		uint32_t size, uint32_t offset, VkShaderStageFlags stageFlags)
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
		std::vector<DescriptorSetLayout> layouts;

		for (const auto& bindings: mBindings | std::views::values)
		{
			VkDescriptorSetLayoutCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.bindingCount = static_cast<uint32_t>(bindings.size()),
				.pBindings = bindings.data()
			};

			VkDescriptorSetLayout handle = VK_NULL_HANDLE;
			Core::AssertEQ(
						   vkCreateDescriptorSetLayout(mDevice->Handle(), &createInfo, nullptr, &handle),
						   VK_SUCCESS);

			DescriptorSetLayout layout(handle);
			for (auto& binding : bindings)
			{
				layout.RecordBinding(binding);
			}

			layouts.emplace_back(layout);
		}

		std::vector<VkDescriptorSetLayout>
				layoutArray = layouts
							  | std::views::transform([](const auto& x) ->
												  VkDescriptorSetLayout {
														  return x.Handle();
													  })
							  | std::ranges::to<std::vector>();

		VkPipelineLayoutCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = static_cast<uint32_t>(layouts.size()),
			.pSetLayouts = layoutArray.data(),
			.pushConstantRangeCount = static_cast<uint32_t>(mPushConstantRanges.size()),
			.pPushConstantRanges = mPushConstantRanges.data()
		};


		VkPipelineLayout handle = VK_NULL_HANDLE;
		Core::AssertEQ(
					   vkCreatePipelineLayout(mDevice->Handle(), &createInfo, nullptr, &handle),
					   VK_SUCCESS);


		return PipelineLayout(handle, *mDevice, std::move(layouts));
	}
}
