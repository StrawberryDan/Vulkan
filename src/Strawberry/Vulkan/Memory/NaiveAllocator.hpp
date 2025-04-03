#pragma once


#include "Allocator.hpp"


namespace Strawberry::Vulkan
{
	class NaiveAllocator
		: public Allocator
	{
	public:
		NaiveAllocator(const Device& device, MemoryTypeIndex memoryType);

		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(Allocation&& address) noexcept override;

	private:
		MemoryTypeIndex mMemoryTypeIndex;
		std::unordered_map<Address, MemoryPool> mMemoryPools;
	};
}
