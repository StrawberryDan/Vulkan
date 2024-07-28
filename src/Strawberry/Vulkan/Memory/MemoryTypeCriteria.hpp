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
		uint32_t              typeMask;
		VkMemoryPropertyFlags requiredProperties;
		VkMemoryPropertyFlags preferredProperties;


		static MemoryTypeCriteria Null();
		static MemoryTypeCriteria DeviceLocal();
		static MemoryTypeCriteria HostVisible();
	};
}
