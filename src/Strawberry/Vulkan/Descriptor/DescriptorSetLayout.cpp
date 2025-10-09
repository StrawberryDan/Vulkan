#include "DescriptorSetLayout.hpp"


namespace Strawberry::Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(VkDescriptorSetLayout layout)
		: mLayout(layout) {}


	VkDescriptorSetLayout DescriptorSetLayout::Handle() const
	{
		return mLayout;
	}


	std::vector<VkDescriptorPoolSize> DescriptorSetLayout::GetPoolSizes() const
	{
		return mPoolSizes;
	}


	void DescriptorSetLayout::RecordBinding(const VkDescriptorSetLayoutBinding& binding)
	{
		bool found = false;

		for (auto& [type, count]: mPoolSizes)
		{
			if (type == binding.descriptorType)
			{
				count += binding.descriptorCount;
				found = true;
			}
		}

		if (!found)
		{
			VkDescriptorPoolSize poolSize
			{
				.type = binding.descriptorType,
				.descriptorCount = binding.descriptorCount
			};
			mPoolSizes.emplace_back(poolSize);
		}
	}
}
