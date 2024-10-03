#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Straberry Vulkan
#include "Strawberry/Vulkan/Memory/Allocator.hpp"
#include "Strawberry/Vulkan/Memory/NaiveAllocator.hpp"
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
		FreeListAllocator(Device& device, uint32_t memoryType, std::uintptr_t size);


		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(Allocation&& address) noexcept override;

	private:
		struct FreeRegion
		{
			uintptr_t offset;
			size_t    size;
		};


		NaiveAllocator mMemoryPoolAllocator;
		MemoryPool     mMemoryPool;

		using RegionID = uint64_t;

		RegionID                       mNextRegionID = 0;
		std::map<RegionID, FreeRegion> mRegions;
		std::list<RegionID>            mRegionsByOffset;
		std::list<RegionID>            mRegionsBySize;

		RegionID   AddFreeRegion(FreeRegion region);
		FreeRegion RemoveRegion(RegionID id);
		void ExpandBlock(RegionID id);
		bool AreBlocksContiguouse(RegionID a, RegionID b) const noexcept;
		void MergeBlocks(const std::list<RegionID>& regions) noexcept;


		decltype(mRegionsByOffset)::const_iterator RegionIDFromOffset(size_t offset);
		decltype(mRegionsBySize)::const_iterator   FindInSizeList(RegionID id) const;
		decltype(mRegionsByOffset)::const_iterator FindInOffsetList(RegionID id) const;
	};
}
