#pragma once


#include "Strawberry/Vulkan/Device.hpp"


namespace Strawberry::Vulkan
{
	struct AllocationRequest
	{
		AllocationRequest(size_t size, size_t alignment)
			: size(size)
			, alignment(alignment) {}


		AllocationRequest(size_t size, size_t alignment, uint32_t typeMask)
			: size(size)
			, alignment(alignment)
			, typeMask(typeMask) {}


		AllocationRequest(const VkMemoryRequirements& requirements)
			: size(requirements.size)
			, alignment(requirements.alignment)
			, typeMask(requirements.memoryTypeBits) {}


		size_t                         size;
		size_t                         alignment;
		uint32_t                       typeMask = -1;
	};
}