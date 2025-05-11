#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Device.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <map>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class PipelineLayout
			: public Core::EnableReflexivePointer
	{
	public:
		class Builder;


		PipelineLayout(const PipelineLayout&) = delete;
		PipelineLayout(PipelineLayout&&);
		PipelineLayout& operator=(const PipelineLayout&) = delete;
		PipelineLayout& operator=(PipelineLayout&&);
		~PipelineLayout();


		operator VkPipelineLayout() const;


		VkDescriptorSetLayout GetSetLayout(uint32_t index);

	protected:
		PipelineLayout(VkPipelineLayout handle, Core::ReflexivePointer<Device> device, std::vector<VkDescriptorSetLayout> setLayouts);

	private:
		VkPipelineLayout                   mHandle;
		Core::ReflexivePointer<Device>     mDevice;
		std::vector<VkDescriptorSetLayout> mSetLayouts;
	};


	class PipelineLayout::Builder
	{
	public:
		Builder(Device& device);
		Builder(const Builder&) = delete;
		Builder(Builder&&);
		Builder& operator=(const Builder&) = delete;
		Builder& operator=(Builder&&);


		Builder& WithDescriptor(unsigned int set, VkDescriptorType type, VkShaderStageFlags shaderStages, unsigned int count = 1);


		Builder& WithPushConstantRange(uint32_t size, uint32_t offset, VkShaderStageFlags stageFlags);


		PipelineLayout Build();


	private:
		Core::ReflexivePointer<Device> mDevice;

		// Maps set indices to a list of bindings.
		std::map<unsigned int, std::vector<VkDescriptorSetLayoutBinding>> mBindings;
		std::vector<VkPushConstantRange>   mPushConstantRanges;
	};
}
