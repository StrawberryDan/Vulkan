#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <ranges>

#include "Strawberry/Core/Types/NullValue.hpp"
#include "Strawberry/Vulkan/Memory/Allocator.hpp"


namespace Strawberry::Vulkan
{
	class BuddyAllocator
			: public Allocator
	{
	public:
		static constexpr size_t MIN_BLOCK_SIZE = 64;


		BuddyAllocator(MemoryPool&& memoryPool);


		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override;
		void             Free(Allocation&& address) noexcept override;

		size_t SpaceAvailable() const noexcept override;

	private:
		using BlockIndex = Core::NonMax<unsigned int>;


		struct Block
		{
			Block() = default;


			Block(uintptr_t offset, size_t size)
				: offset(offset)
				, size(size) {}


			bool HasChildren() const
			{
				Core::Assert(leftChild.HasValue() == rightChild.HasValue());
				return leftChild.HasValue();
			}


			bool                       allocated         = false;
			bool                       allocatedChildren = false;
			uintptr_t                  offset;
			size_t                     size;
			Core::Optional<BlockIndex> parent;
			Core::Optional<BlockIndex> leftChild;
			Core::Optional<BlockIndex> rightChild;
		};


		Block* Root()
		{
			return &mBlocks.at(0);
		}


		Block* GetBlock(BlockIndex index)
		{
			return &mBlocks.at(index);
		}


		std::pair<BlockIndex, BlockIndex> SplitBlock(BlockIndex index);


		BlockIndex::Inner                  mNextBlockIndex = 1;
		std::map<BlockIndex::Inner, Block> mBlocks;
		size_t                             mSpaceAllocated = 0;
	};
}
