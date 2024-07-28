//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "MemoryTypeCriteria.hpp"


namespace Strawberry::Vulkan
{
	MemoryTypeCriteria MemoryTypeCriteria::DeviceLocal()
	{
		return {.requiredProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, .preferredProperties = 0};
	}


	MemoryTypeCriteria MemoryTypeCriteria::HostVisible()
	{
		return {.requiredProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, .preferredProperties = 0};
	}
}
