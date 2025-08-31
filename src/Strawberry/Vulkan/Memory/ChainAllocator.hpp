#pragma once
#include <unordered_set>

#include "Allocator.hpp"


namespace Strawberry::Vulkan
{
	template <std::derived_from<PoolAllocator> T>
	class ChainAllocator
		: public SingleAllocator
	{
	public:
		ChainAllocator(Device& device, MemoryTypeIndex memoryTypeIndex, size_t poolSize) noexcept
			: SingleAllocator(device, memoryTypeIndex)
			, mPoolSize(poolSize)
		{
			ExtendChain();
		}

		void Free(Allocation&& allocation) noexcept override
		{
			for (auto& allocator : mAllocatorChain)
			{
				if (allocator.allocations.contains(allocation.Address()))
				{
					allocator.allocator.Free(std::move(allocation));
				}
			}

			Core::Unreachable();
		}


		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override
		{
			for (auto& allocator : mAllocatorChain)
			{
				auto result = allocator.allocator.Allocate(allocationRequest);

				if (result)
				{
					allocator.allocations.emplace(result.Value().Address());
					return result;
				}

				if (!result.Err().template IsType<AllocationError::OutOfMemory>())
				{
					return result;
				}
			}

			ExtendChain();
			return mAllocatorChain.back().allocator.Allocate(allocationRequest);
		}


	private:
		void ExtendChain();


		struct ChainItem
		{
			std::unordered_set<Address> allocations;
			T allocator;
		};


		size_t mPoolSize;
		std::vector<ChainItem> mAllocatorChain;
	};


	template <std::derived_from<PoolAllocator> T>
	void ChainAllocator<T>::ExtendChain()
	{
		ChainItem item {
			.allocations{},
			.allocator{ T(MemoryPool::Allocate(GetDevice(), GetMemoryTypeIndex(), mPoolSize).Unwrap()) }
		};

		mAllocatorChain.emplace_back(std::move(item));
	}
}
