#pragma once
#include "SingleAllocator.hpp"


namespace Strawberry::Vulkan
{
	class PoolAllocator
		: public SingleAllocator
	{
	public:
		PoolAllocator(MemoryPool&& memoryPool)
			: SingleAllocator(*memoryPool.GetDevice(), memoryPool.GetMemoryTypeIndex())
			, mMemoryPool(std::move(memoryPool))
		{}


		const MemoryPool& Memory() const noexcept { return mMemoryPool; }
		MemoryPool& Memory()       noexcept { return mMemoryPool; }


	private:
		MemoryPool mMemoryPool;
	};
}