#include "DescriptorPoolAllocator.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include <utility>

#include "Strawberry/Vulkan/Pipeline/PipelineLayout.hpp"


namespace Strawberry::Vulkan
{
	DescriptorPoolAllocator::DescriptorPoolAllocator(Device& device) {}


	DescriptorSet DescriptorPoolAllocator::Allocate(Device& device, DescriptorSetLayout layout)
	{
		return mPoolLists[layout.Handle()].Allocate(device, std::move(layout));
	}


	DescriptorPoolAllocator::Pool DescriptorPoolAllocator::Pool::Create(Device& device, std::vector<VkDescriptorPoolSize> poolSizes)
	{
		return Pool{ .descriptorPool = DescriptorPool(device, 0, DESCRIPTOR_POOL_SIZE, std::move(poolSizes)) };

	}


	DescriptorSet DescriptorPoolAllocator::Pool::Allocate(Device& device, DescriptorSetLayout layout)
	{
		return DescriptorSet(descriptorPool, layout);
	}


	uint32_t DEFAULT_DESCRIPTOR_COUNT = 1024;


	DescriptorPoolAllocator::PoolList DescriptorPoolAllocator::PoolList::Create(Device& device, VkDescriptorSetLayout layout, std::vector<VkDescriptorPoolSize> poolSizes)
	{
		PoolList poolList{ .layout = layout, .pools = {} };
		poolList.layout = layout;
		poolList.pools.emplace_back(0, Pool::Create(device, std::move(poolSizes)));
		return poolList;
	}


	DescriptorSet DescriptorPoolAllocator::PoolList::Allocate(Device& device, const DescriptorSetLayout& layout)
	{
		Core::Optional<DescriptorSet> result;

		for (auto& [available, pool]: pools)
		{
			if (available < pool.DESCRIPTOR_POOL_SIZE)
			{
				result = pool.Allocate(device, layout);
			}
		}

		if (!result)
		{
			pools.emplace_back(std::make_pair(0, Pool::Create(device, layout.GetPoolSizes())));
			return pools.back().second.Allocate(device, layout);
		}

		return result.Unwrap();
	}
}
