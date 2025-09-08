#pragma once


namespace Strawberry::Vulkan
{
	class PolyAllocator
		: public Allocator
	{
	public:
		using Allocator::Allocator;

		~PolyAllocator() override = default;

		virtual AllocationResult Allocate(const AllocationRequest& allocationRequest, const MemoryTypeCriteria& memoryTypeCriteria) noexcept = 0;
	};
}