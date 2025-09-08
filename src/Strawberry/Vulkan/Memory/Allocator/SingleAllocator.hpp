#pragma once

#include "Strawberry/Vulkan/Memory/Allocation.hpp"
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/Allocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/AllocationRequest.hpp"


namespace Strawberry::Vulkan
{
	using AllocationResult = Core::Result<Allocation, AllocationError>;


	class SingleAllocator
			: public Allocator
	{
	public:
		SingleAllocator(Device& device, MemoryTypeIndex memoryTypeIndex);

		virtual AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept = 0;


		const MemoryTypeIndex GetMemoryTypeIndex() const noexcept;

	private:
		MemoryTypeIndex mMemoryTypeIndex;
	};
}
