#include "BuddyAllocator.hpp"

#include <deque>


namespace Strawberry::Vulkan
{
	BuddyAllocator::BuddyAllocator(MemoryPool&& memoryPool, size_t minAllocation)
		: mMemoryPool(std::move(memoryPool))
		, mMinGranularity(minAllocation)
	{
		Core::Assert(std::has_single_bit(mMemoryPool.Size()));
		Core::Assert(mMemoryPool.Size() > 2 * MIN_BLOCK_SIZE);
		Core::Assert(std::has_single_bit(minAllocation));
		Core::Assert(minAllocation > MIN_BLOCK_SIZE);

		mBlocks.emplace(0, Block(0, mMemoryPool.Size()));
	}


	AllocationResult BuddyAllocator::Allocate(const AllocationRequest& allocationRequest) noexcept
	{
		auto chosenBlock = [&, this]() -> Core::Optional<BlockIndex::Inner>
		{
			std::deque<BlockIndex> searchList{0};

			while (!searchList.empty())
			{
				BlockIndex::Inner cursor = searchList.front();
				searchList.pop_front();
				Block* block = GetBlock(cursor);
				if (block->offset % allocationRequest.alignment != 0)
					return Core::NullOpt;

				if (block->allocated || block->size < allocationRequest.size)
					continue;

				if ((block->size < 2 * allocationRequest.size || block->size == mMinGranularity) && !block->allocatedChildren)
				{
					return cursor;
				}

				if (block->size > mMinGranularity)
				{
					if (!block->HasChildren())
						SplitBlock(cursor);

					BlockIndex left  = block->leftChild.Value();
					BlockIndex right = block->rightChild.Value();

					searchList.push_front(right);
					searchList.push_front(left);
				}
			}

			return Core::NullOpt;
		}();

		if (!chosenBlock)
		{
			return AllocationError::OutOfMemory();
		}

		Block* block = GetBlock(chosenBlock.Value());

		block->allocated         = true;
		block->allocatedChildren = true;
		Allocation allocation    = mMemoryPool.AllocateView(*this, block->offset, block->size);
		mSpaceAllocated += block->size;


		for (Core::Optional<BlockIndex> cursor = block->parent; cursor;)
		{
			Block* block = GetBlock(cursor.Value());

			if (!block->allocatedChildren)
			{
				unsigned allocatedChildren = 0;
				if (block->leftChild)
				{
					allocatedChildren += GetBlock(block->leftChild.Value())->allocatedChildren ? 1 : 0;
				}
				if (block->rightChild)
				{
					allocatedChildren += GetBlock(block->rightChild.Value())->allocatedChildren ? 1 : 0;
				}

				block->allocatedChildren = allocatedChildren > 0;
				if (!block->allocatedChildren)
				{
					break;
				}
			}
			else
			{
				break;
			}

			cursor = block->parent;
		}

		return allocation;
	}


	void BuddyAllocator::Free(Allocation&& address) noexcept
	{
		BlockIndex::Inner blockIndex = [&]()
		{
			Core::Optional<BlockIndex> cursor = 0;
			while (cursor)
			{
				Block* block = GetBlock(cursor.Value());

				if (block->allocated)
				{
					Core::Assert(block->offset == address.Offset());
					return cursor.Value();
				}

				if (block->HasChildren())
				{
					if (address.Offset() >= block->offset + block->size / 2)
					{
						cursor = block->rightChild.Value();
					}
					else
					{
						cursor = block->leftChild.Value();
					}
				}
			}

			Core::Unreachable();
		}();

		Block* block             = GetBlock(blockIndex);
		block->allocated         = false;
		block->allocatedChildren = false;
		mSpaceAllocated -= block->size;

		for (Core::Optional<BlockIndex> cursor = block->parent; cursor;)
		{
			Block* block = GetBlock(cursor.Value());

			if (block->allocatedChildren)
			{
				unsigned allocatedChildren = 0;
				if (block->leftChild)
				{
					allocatedChildren += GetBlock(block->leftChild.Value())->allocatedChildren ? 1 : 0;
				}
				if (block->rightChild)
				{
					allocatedChildren += GetBlock(block->rightChild.Value())->allocatedChildren ? 1 : 0;
				}

				block->allocatedChildren = allocatedChildren > 0;
				if (block->allocatedChildren)
				{
					break;
				}
			}
			else
			{
				break;
			}

			cursor = block->parent;
		}
	}


	std::pair<BuddyAllocator::BlockIndex, BuddyAllocator::BlockIndex> BuddyAllocator::SplitBlock(BlockIndex index)
	{
		Block* parent = GetBlock(index);
		Core::Assert(!parent->allocated);
		Core::Assert(!parent->HasChildren());
		size_t size = parent->size / 2;

		Block left(parent->offset, size);
		left.parent = index;
		Block right(parent->offset + size, size);
		right.parent = index;

		BlockIndex leftIndex  = mNextBlockIndex++;
		BlockIndex rightIndex = mNextBlockIndex++;

		mBlocks.emplace(leftIndex, left);
		mBlocks.emplace(rightIndex, right);

		parent->leftChild  = leftIndex;
		parent->rightChild = rightIndex;

		return std::make_pair(leftIndex, rightIndex);
	}
}
