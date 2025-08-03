#include "FreelistAllocator.hpp"

#include <ranges>
#include <algorithm>


namespace Strawberry::Vulkan
{
	FreeListAllocator::FreeListAllocator(MemoryPool&& memoryPool)
		: PoolAllocator(std::move(memoryPool))
	{
		AddFreeRegion(FreeRegion{.offset = 0, .size = Memory().Size()});
	}


	AllocationResult FreeListAllocator::Allocate(const AllocationRequest& allocationRequest) noexcept
	{
		if (Memory().Size() < allocationRequest.size) [[unlikely]]
		{
			return AllocationError::InsufficientPoolSize{};
		}

		// Make sure that this is one of the valid memory types for this allocation.
		Core::Assert(allocationRequest.typeMask & (1 << Memory().GetMemoryTypeIndex().memoryTypeIndex));
		// Function for calculating the next aligned address at a position.
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
			for (auto [offset, region] : mRegions)
			{
				if (Core::Optional<unsigned int> alignedAddress = AlignedAddress(region.offset,
				                                                                 region.size,
				                                                                 allocationRequest.alignment))
				{
					return RemoveRegion(offset);
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
		// Create allocation in the segment of the region.
		Allocation result = Memory().AllocateView(*this, alignedAddress, allocationRequest.size);

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

		return result;
	}


	void FreeListAllocator::Free(Allocation&& address) noexcept
	{
		AddFreeRegion(FreeRegion{.offset = address.Offset(), .size = address.Size()});
		ExpandBlock(address.Offset());
	}


	void FreeListAllocator::AddFreeRegion(FreeRegion region)
	{
		// Insert region into list
		mRegions.emplace(region.offset, region);
	}


	FreeListAllocator::FreeRegion FreeListAllocator::RemoveRegion(Offset offset)
	{
		FreeRegion region = mRegions.at(offset);
		mRegions.erase(offset);
		return region;
	}


	void FreeListAllocator::ExpandBlock(Offset id)
	{
		auto offsetPosition = mRegions.find(id);

		std::list<Offset> contiguousBlocks{id};
		auto              forwardCursor = offsetPosition;
		for (; std::next(forwardCursor) != mRegions.end(); ++forwardCursor)
		{
			Offset a = forwardCursor->first;
			Offset b = std::next(forwardCursor)->first;
			if (AreBlocksContiguous(a, b))
			{
				contiguousBlocks.emplace_back(b);
			}
			else
			{
				break;
			}
		}

		auto backwardCursor = std::make_reverse_iterator(offsetPosition);
		for (; backwardCursor != mRegions.rend(); ++backwardCursor)
		{
			Offset a = std::prev(backwardCursor)->first;
			Offset b = backwardCursor->first;
			if (AreBlocksContiguous(b, a))
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


	bool FreeListAllocator::AreBlocksContiguous(Offset a, Offset b) const noexcept
	{
		return mRegions.at(a).offset + mRegions.at(a).size == mRegions.at(b).offset;
	}


	void FreeListAllocator::MergeBlocks(const std::list<Offset>& regions) noexcept
	{
		auto offsets = regions | std::views::transform([this](Offset id)
		{
			return mRegions.at(id).offset;
		});
		uintptr_t minOffset = *std::ranges::min_element(offsets);

		size_t sumSizes = std::ranges::fold_left(regions | std::views::transform([this](Offset id)
		{
			return mRegions.at(id).size;
		}), 0, std::plus());

		FreeRegion accumulator{.offset = minOffset, .size = sumSizes};

		for (const auto id : regions)
		{
			RemoveRegion(id);
		}

		AddFreeRegion(accumulator);
	}
}
