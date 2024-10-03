#include "FreelistAllocator.hpp"

#include <ranges>
#include <algorithm>


namespace Strawberry::Vulkan
{
	FreeListAllocator::FreeListAllocator(MemoryPool&& memoryPool)
		: Allocator(std::move(memoryPool))
	{
		AddFreeRegion(FreeRegion{.offset = 0, .size = GetMemoryPool().Size()});
	}


	AllocationResult FreeListAllocator::Allocate(const AllocationRequest& allocationRequest) noexcept
	{
		// Function for calculating the next aligned address at a postition.
		auto AlignedAddress = [](unsigned int offset, unsigned int size, unsigned int alignment) -> Core::Optional<unsigned int>
		{
			unsigned int offsetDifference;
			unsigned int modulo = offset % alignment;
			if (modulo == 0) offsetDifference = 0;
			else offsetDifference             = alignment - modulo;
			if (offset + offsetDifference >= size) return Core::NullOpt;
			return offset + offsetDifference;
		};

		// Find a suitable region.
		Core::Optional<FreeRegion> region = [&]()-> Core::Optional<FreeRegion>
		{
			// Find the first region which may be large enough;
			auto smallestCandidateRegion = std::lower_bound(mRegionsBySize.begin(), mRegionsBySize.end(), allocationRequest.size,
			                                                [this](const RegionID& a, const size_t& b)
			                                                {
				                                                return mRegions.at(a).size < b;
			                                                });
			// If no region is large enough, return an error.
			if (smallestCandidateRegion == mRegionsBySize.end())
			{
				return Core::NullOpt;
			}


			for (; smallestCandidateRegion != mRegionsBySize.end(); ++smallestCandidateRegion)
			{
				if (Core::Optional<unsigned int> alignedAddress = AlignedAddress(mRegions.at(*smallestCandidateRegion).offset,
				                                                                 mRegions.at(*smallestCandidateRegion).size,
				                                                                 allocationRequest.alignment))
				{
					return RemoveRegion(*smallestCandidateRegion);
				}
			}

			return Core::NullOpt;
		}();

		// If no region found then we are out of memory.
		if (!region)
		{
			return AllocationError::OutOfMemory();
		}

		unsigned int alignedAddress      = AlignedAddress(region->offset, region->size, allocationRequest.alignment).Unwrap();
		unsigned int alignmentDifference = alignedAddress - region->offset;
		// Create allocation in segment of region.
		Allocation result = GetMemoryPool().AllocateView(*this, alignedAddress, allocationRequest.size);

		// Track skipped padding
		const FreeRegion priorRegion{.offset = region->offset, .size = alignmentDifference};
		if (priorRegion.size > 0)
		{
			AddFreeRegion(priorRegion);
		}

		// Tracked allocated object
		const FreeRegion proceedingRegion{
			.offset = region->offset + alignmentDifference + allocationRequest.size,
			.size = region->size - alignmentDifference - allocationRequest.size
		};
		if (proceedingRegion.size > 0)
		{
			AddFreeRegion(proceedingRegion);
		}

		Core::AssertEQ(priorRegion.size + proceedingRegion.size + allocationRequest.size, region->size);

		mSpaceAllocated += result.Size();
		return result;
	}


	void FreeListAllocator::Free(Allocation&& address) noexcept
	{
		mSpaceAllocated -= address.Size();
		RegionID freshBlock = AddFreeRegion(FreeRegion{.offset = address.Offset(), .size = address.Size()});
		ExpandBlock(freshBlock);
	}


	size_t FreeListAllocator::SpaceAvailable() const noexcept
	{
		return Capacity() - mSpaceAllocated;
	}


	FreeListAllocator::RegionID FreeListAllocator::AddFreeRegion(FreeRegion region)
	{
		// Insert with new id.
		const RegionID newID = mNextRegionID++;
		mRegions.emplace(newID, region);

		// Insert id into size list
		const auto sizePos = std::lower_bound(mRegionsBySize.begin(), mRegionsBySize.end(), region.size, [this](RegionID region, size_t size)
		{
			return mRegions.at(region).size < size;
		});
		mRegionsBySize.emplace(sizePos, newID);

		// Insert id into offset list
		const auto offsetPos = std::lower_bound(mRegionsByOffset.begin(), mRegionsByOffset.end(), region.offset, [this](RegionID region, size_t offset)
		{
			return mRegions.at(region).offset < offset;
		});
		mRegionsByOffset.emplace(offsetPos, newID);
		// Return result.
		return newID;
	}


	FreeListAllocator::FreeRegion FreeListAllocator::RemoveRegion(RegionID id)
	{
		mRegionsBySize.erase(FindInSizeList(id));
		mRegionsByOffset.erase(FindInOffsetList(id));
		FreeRegion region = mRegions.at(id);
		mRegions.erase(id);
		return region;
	}


	void FreeListAllocator::ExpandBlock(RegionID id)
	{
		auto offsetPosition = FindInOffsetList(id);

		std::list<RegionID> contiguousBlocks{*offsetPosition};
		auto                forwardCursor = offsetPosition;
		for (; std::next(forwardCursor) != mRegionsByOffset.end(); ++forwardCursor)
		{
			RegionID a = *forwardCursor;
			RegionID b = *std::next(forwardCursor);
			if (AreBlocksContiguouse(a, b))
			{
				contiguousBlocks.emplace_back(b);
			}
			else
			{
				break;
			}
		}

		auto backwardCursor = std::make_reverse_iterator(offsetPosition);
		for (; backwardCursor != mRegionsByOffset.rend(); ++backwardCursor)
		{
			RegionID a = *std::prev(backwardCursor);
			RegionID b = *backwardCursor;
			if (AreBlocksContiguouse(b, a))
			{
				contiguousBlocks.emplace_front(b);
			}
			else
			{
				break;
			}
		}

		if (contiguousBlocks.size() > 1)
		{
			MergeBlocks(contiguousBlocks);
		}
	}


	bool FreeListAllocator::AreBlocksContiguouse(RegionID a, RegionID b) const noexcept
	{
		return mRegions.at(a).offset + mRegions.at(a).size == mRegions.at(b).offset;
	}


	void FreeListAllocator::MergeBlocks(const std::list<RegionID>& regions) noexcept
	{
		auto offsets = regions | std::views::transform([this](RegionID id)
		{
			return mRegions.at(id).offset;
		});
		uintptr_t minOffset = *std::ranges::min_element(offsets);

		size_t sumSizes = std::ranges::fold_left_first(regions | std::views::transform([this](RegionID id)
		{
			return mRegions.at(id).size;
		}), std::plus()).value();

		FreeRegion accumulator{.offset = minOffset, .size = sumSizes};

		for (const auto id : regions)
		{
			RemoveRegion(id);
		}

		AddFreeRegion(accumulator);
	}


	decltype(FreeListAllocator::mRegionsByOffset)::const_iterator FreeListAllocator::RegionIDFromOffset(size_t offset)
	{
		return std::lower_bound(mRegionsByOffset.begin(), mRegionsByOffset.end(), offset, [this](RegionID a, size_t offset)
		{
			return mRegions.at(a).offset < offset;
		});
	}


	decltype(FreeListAllocator::mRegionsBySize)::const_iterator FreeListAllocator::FindInSizeList(RegionID id) const
	{
		return std::lower_bound(mRegionsBySize.begin(), mRegionsBySize.end(), id, [this](RegionID a, RegionID b)
		{
			return mRegions.at(a).size < mRegions.at(b).size;
		});
	}


	decltype(FreeListAllocator::mRegionsByOffset)::const_iterator FreeListAllocator::FindInOffsetList(RegionID id) const
	{
		return std::lower_bound(mRegionsByOffset.begin(), mRegionsByOffset.end(), id, [this](RegionID a, RegionID b)
		{
			return mRegions.at(a).offset < mRegions.at(b).offset;
		});
	}
}
