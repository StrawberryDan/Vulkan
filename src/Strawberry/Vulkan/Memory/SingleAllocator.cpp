#include "SingleAllocator.hpp"


namespace Strawberry::Vulkan
{
	SingleAllocator::SingleAllocator(Device& device, MemoryTypeIndex memoryTypeIndex)
		: Allocator(device)
		  , mMemoryTypeIndex(memoryTypeIndex) {}


	const MemoryTypeIndex SingleAllocator::GetMemoryTypeIndex() const noexcept
	{
		return mMemoryTypeIndex;
	}
}
