#pragma once
#include "SingleAllocator.hpp"


namespace Strawberry::Vulkan
{
	class NaiveAllocator
		: public SingleAllocator
	{
	public:
		NaiveAllocator(Device& device, MemoryTypeIndex memoryType);

		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(Allocation&& address) noexcept override;

	private:
		std::unordered_map<Address, MemoryPool> mMemoryPools;
	};
}
