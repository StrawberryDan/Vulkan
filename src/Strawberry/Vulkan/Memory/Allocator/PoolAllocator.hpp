#pragma once
#include "SingleAllocator.hpp"


namespace Strawberry::Vulkan
{
	class PoolAllocator
		: public SingleAllocator
	{
	public:
		explicit PoolAllocator(MemoryPool&& memoryPool);

		const MemoryPool& Memory() const noexcept;

		MemoryPool& Memory() noexcept;


	private:
		MemoryPool mMemoryPool;
	};
}