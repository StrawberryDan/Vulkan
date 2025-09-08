#pragma once
#include "MonoAllocator.hpp"


namespace Strawberry::Vulkan
{
	class PoolAllocator
		: public MonoAllocator
	{
	public:
		explicit PoolAllocator(MemoryPool&& memoryPool);

		const MemoryPool& Memory() const noexcept;

		MemoryPool& Memory() noexcept;


	private:
		MemoryPool mMemoryPool;
	};
}