#include "Memory.hpp"
#include <vulkan/vulkan.h>


namespace Strawberry::Vulkan
{
	VkMemoryPropertyFlags MemoryTypeIndex::GetProperties() const noexcept
	{
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		return memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
	}
}