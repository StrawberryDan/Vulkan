#include "PoolAllocator.hpp"


namespace Strawberry::Vulkan
{
	PoolAllocator::PoolAllocator(MemoryPool&& memoryPool)
			: SingleAllocator(memoryPool.GetDevice(), memoryPool.GetMemoryTypeIndex())
			, mMemoryPool(std::move(memoryPool))
	{}

	const MemoryPool& PoolAllocator::Memory() const noexcept
	{
		return mMemoryPool;
	}

	MemoryPool& PoolAllocator::Memory() noexcept
	{
		return mMemoryPool;
	}
}
