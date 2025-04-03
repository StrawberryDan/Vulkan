#include "NaiveAllocator.hpp"


namespace Strawberry::Vulkan
{
	NaiveAllocator::NaiveAllocator(const Device& device, MemoryTypeIndex memoryType)
		: Allocator(device)
		  , mMemoryTypeIndex(memoryType)
	{
	}

	AllocationResult NaiveAllocator::Allocate(const AllocationRequest& allocationRequest) noexcept
	{
		MemoryPool memoryPool = MemoryPool::Allocate(*GetDevice(), mMemoryTypeIndex, allocationRequest.size).Unwrap();

		Allocation allocation = memoryPool.AllocateView(*this, 0, memoryPool.Size());
		mMemoryPools.emplace(allocation.Address(), std::move(memoryPool));
		return allocation;
	}

	void NaiveAllocator::Free(Allocation&& address) noexcept
	{
		mMemoryPools.erase(address.Address());
	}
}
