#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Allocator.hpp"
// Standard Library
#include <set>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class NaiveAllocator
			: public Allocator
	{
	public:
		using RawAllocationResult = Core::Result<MemoryPool, AllocationError>;


		NaiveAllocator(Device& device, uint32_t memoryType);


		RawAllocationResult AllocateRaw(size_t size) noexcept;


		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(Allocation&& address) noexcept override;

	private:
		std::map<VkDeviceMemory, MemoryPool> mAllocations;
	};
}
