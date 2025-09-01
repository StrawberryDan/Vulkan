#pragma once
#include "Allocation.hpp"
#include "Strawberry/Core/Types/Result.hpp"


namespace Strawberry::Vulkan
{
	using AllocationResult = Core::Result<Allocation, AllocationError>;


	class SingleAllocator
		: public Allocator
	{
	public:
		SingleAllocator(Device& device, MemoryTypeIndex memoryTypeIndex)
			: Allocator(device)
			, mMemoryTypeIndex(memoryTypeIndex)
		{}

		virtual AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept = 0;


		const MemoryTypeIndex GetMemoryTypeIndex() const noexcept { return mMemoryTypeIndex; }


	private:
		MemoryTypeIndex mMemoryTypeIndex;
	};
}