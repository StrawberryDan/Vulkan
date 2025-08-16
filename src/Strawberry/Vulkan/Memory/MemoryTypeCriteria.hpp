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
		VkMemoryPropertyFlags requiredProperties = 0;
		VkMemoryPropertyFlags preferredProperties = 0;


		static MemoryTypeCriteria Null();
		static MemoryTypeCriteria DeviceLocal();
		static MemoryTypeCriteria HostVisible();


		MemoryTypeCriteria& WithMinimumSize(size_t minimumSize)
		{
			this->minimumSize = minimumSize;
			return *this;
		}
	};
}
