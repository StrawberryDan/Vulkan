#pragma once
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

#include "Strawberry/Vulkan/Descriptor/DescriptorSet.hpp"
#include "Strawberry/Vulkan/Descriptor/DescriptorPool.hpp"
#include "Strawberry/Vulkan/Pipeline/PipelineLayout.hpp"


namespace Strawberry::Vulkan
{
	class DescriptorPoolAllocator
	{
	public:
		DescriptorPoolAllocator(Device& device);


		Result<DescriptorSet> Allocate(Device& device, DescriptorSetLayout layout);

	private:
		struct Pool
		{
			static constexpr uint32_t DESCRIPTOR_POOL_SIZE = 2 * 1024;

			static Pool Create(Device& device, std::vector<VkDescriptorPoolSize> poolSizes);

			Result<DescriptorSet> Allocate(Device& device, DescriptorSetLayout layout);

			DescriptorPool descriptorPool;
		};


		struct PoolList
		{
			static PoolList Create(Device& device, VkDescriptorSetLayout layout, std::vector<VkDescriptorPoolSize> poolSizes);

			Result<DescriptorSet> Allocate(Device& device, const DescriptorSetLayout& layout);

			VkDescriptorSetLayout                       layout;
			std::vector<std::pair<unsigned int, Pool> > pools;
		};


		std::unordered_map<VkDescriptorSetLayout, PoolList> mPoolLists;
	};
}
