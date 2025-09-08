#include "MonoAllocator.hpp"


namespace Strawberry::Vulkan
{
	MonoAllocator::MonoAllocator(Device& device, MemoryTypeIndex memoryTypeIndex)
		: Allocator(device)
		  , mMemoryTypeIndex(memoryTypeIndex) {}


	const MemoryTypeIndex MonoAllocator::GetMemoryTypeIndex() const noexcept
	{
		return mMemoryTypeIndex;
	}
}
