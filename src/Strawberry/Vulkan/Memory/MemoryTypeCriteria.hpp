#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	struct MemoryTypeCriteria
	{
		size_t                minimumSize = 0;
		VkMemoryPropertyFlags requiredProperties;
		VkMemoryPropertyFlags preferredProperties;


		static MemoryTypeCriteria Null();
		static MemoryTypeCriteria DeviceLocal();
		static MemoryTypeCriteria HostVisible();


		MemoryTypeCriteria WithMinimumSize(size_t minimumSize) const
		{
			MemoryTypeCriteria criteria = *this;
			criteria.minimumSize = minimumSize;
			return criteria;
		}
	};
}
