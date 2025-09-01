#pragma once


namespace Strawberry::Vulkan
{
	class MultiAllocator
		: public Allocator
	{
	public:
		using Allocator::Allocator;

		~MultiAllocator() override = default;

		virtual AllocationResult Allocate(const AllocationRequest& allocationRequest, const MemoryTypeCriteria& memoryTypeCriteria) noexcept = 0;
	};
}