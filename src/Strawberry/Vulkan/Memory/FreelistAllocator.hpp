#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Straberry Vulkan
#include "Strawberry/Vulkan/Memory/Allocator.hpp"
// Standard Library
#include <cstdint>
#include <list>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class FreeListAllocator
			: public Allocator
	{
	public:
		FreeListAllocator(MemoryPool&& memoryPool);


		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(Allocation&& address) noexcept override;

		const MemoryPool& Memory() const noexcept { return mMemoryPool; }

	private:
		struct FreeRegion
		{
			uintptr_t offset;
			size_t    size;
		};


		using Offset = uint64_t;

		MemoryPool mMemoryPool;

		// The container of all the regions of free memory
		std::map<Offset, FreeRegion> mRegions;
		// Associates the 
		std::list<Offset>          mRegionsBySize;


		// Functions for managing the list of regions.
		//
		// Add a new free region to the list
		void       AddFreeRegion(FreeRegion region);
		// Remove the region associate with the given ID
		FreeRegion RemoveRegion(Offset id);
		// Expand the given block to fill empty space
		void       ExpandBlock(Offset id);
		// Checks if 2 blocks are adjacent to eachother without any gap. Region A must be to the left of region B.
		bool       AreBlocksContiguous(Offset a, Offset b) const noexcept;
		// Marge the given list to regions into a single block
		void       MergeBlocks(const std::list<Offset>& regions) noexcept;
	};
}
