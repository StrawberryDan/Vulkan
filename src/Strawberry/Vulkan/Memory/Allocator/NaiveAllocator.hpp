#pragma once
#include "MonoAllocator.hpp"


namespace Strawberry::Vulkan
{
	class NaiveAllocator
		: public MonoAllocator
	{
	public:
		NaiveAllocator(Device& device, MemoryTypeIndex memoryType);

		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(MemoryBlock&& address) noexcept override;

	private:
		std::unordered_map<Address, MemoryPool> mMemoryPools;
	};
}
