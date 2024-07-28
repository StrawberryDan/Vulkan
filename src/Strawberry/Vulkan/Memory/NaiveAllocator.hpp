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
		using RawAllocationResult = Core::Result<Allocation, AllocationError>;


		NaiveAllocator(Device& device);


		RawAllocationResult AllocateRaw(size_t size, const MemoryTypeCriteria& criteria) noexcept;


		AllocationResult Allocate(size_t size, const MemoryTypeCriteria& criteria) noexcept override;
		void             Free(AllocationView&& address) noexcept override;

	private:
		std::map<VkDeviceMemory, Allocation> mAllocations;
	};
}
