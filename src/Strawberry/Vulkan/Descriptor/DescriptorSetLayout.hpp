#pragma once

#include <vulkan/vulkan.h>
#include <vector>


namespace Strawberry::Vulkan
{
	class DescriptorSetLayout
	{
	public:
		explicit DescriptorSetLayout(VkDescriptorSetLayout layout);


		[[nodiscard]] VkDescriptorSetLayout Handle() const;

		std::vector<VkDescriptorPoolSize> GetPoolSizes() const;

		void RecordBinding(const VkDescriptorSetLayoutBinding& binding);

	private:
		VkDescriptorSetLayout             mLayout;
		std::vector<VkDescriptorPoolSize> mPoolSizes;
	};
}