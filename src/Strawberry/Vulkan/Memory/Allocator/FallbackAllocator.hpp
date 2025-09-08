#pragma once
#include <unordered_set>

#include "Strawberry/Vulkan/Memory/Allocator/Allocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/NaiveAllocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/ChainAllocator.hpp"


namespace Strawberry::Vulkan
{
	template<std::derived_from<SingleAllocator> T>
	class FallbackAllocator
			: public SingleAllocator
	{
	public:
		FallbackAllocator(T&& allocator)
			: SingleAllocator(allocator.GetDevice(), allocator.GetMemoryTypeIndex())
			  , mMainAllocator(std::move(allocator))
			  , mFallbackAllocator(mMainAllocator.GetDevice(), mMainAllocator.GetMemoryTypeIndex()) {}


		void Free(MemoryBlock&& allocation) noexcept override
		{
			if (mFallbackAllocations.contains(allocation.Address()))
			{
				mFallbackAllocations.erase(allocation.Address());
				mFallbackAllocator.Free(std::move(allocation));
			}
			else
			{
				mMainAllocator.Free(std::move(allocation));
			}
		}


		AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept override
		{
			AllocationResult result = mMainAllocator.Allocate(allocationRequest);
			if (!result)
			{
				const bool allocateFromFallback = result.Err().IsAnyOf<
					AllocationError::InsufficientPoolSize,
					AllocationError::OutOfMemory>();
				if (allocateFromFallback)
				{
					result = mFallbackAllocator.Allocate(allocationRequest);
					if (result)
					{
						mFallbackAllocations.emplace(result.Value().Address());
					}
					return result;
				}
			}


			return result;
		}

	private:
		T                           mMainAllocator;
		NaiveAllocator              mFallbackAllocator;
		std::unordered_set<Address> mFallbackAllocations;
	};


	template<typename T>
	using FallbackChainAllocator = FallbackAllocator<ChainAllocator<T> >;
}
