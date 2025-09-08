#pragma once
#include "Strawberry/Vulkan/Memory/Allocator/MonoAllocator.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/PolyAllocator.hpp"
#include <unordered_set>


namespace Strawberry::Vulkan
{
	template<std::derived_from<MonoAllocator> Base>
	class NaivePolyAllocator
			: public PolyAllocator
	{
	public:
		explicit NaivePolyAllocator(Device& device)
			: PolyAllocator(device) {}

		~NaivePolyAllocator() override = default;

		MonoAllocator& GetAllocator(MemoryTypeIndex typeIndex);

		AllocationResult Allocate(const AllocationRequest&  allocationResult,
								  const MemoryTypeCriteria& memoryTypeCriteria) noexcept override;

		void Free(MemoryBlock&& address) noexcept override;

	private:
		struct BaseAllocator
		{
			Base                        allocator;
			std::unordered_set<Address> mAllocatedAddresses;
		};

		std::map<MemoryTypeIndex, BaseAllocator> mAllocators;
	};


	template<std::derived_from<MonoAllocator> Base>
	MonoAllocator& NaivePolyAllocator<Base>::GetAllocator(MemoryTypeIndex typeIndex)
	{
		if (!mAllocators.contains(typeIndex)) [[unlikely]]
		{
			mAllocators.emplace(typeIndex, Base{ { GetDevice(), typeIndex, size_t{ 4 * 1024 * 1024 } } });
		}

		return mAllocators.at(typeIndex).allocator;
	}

	template<std::derived_from<MonoAllocator> Base>
	AllocationResult NaivePolyAllocator<Base>::Allocate(
		const AllocationRequest&  allocationResult,
		const MemoryTypeCriteria& memoryTypeCriteria) noexcept
	{
		auto candidateMemoryTypes =
				GetDevice().GetPhysicalDevice().SearchMemoryTypes(memoryTypeCriteria);

		auto selectedMemoryType = candidateMemoryTypes[0];

		auto result = GetAllocator(selectedMemoryType.index).Allocate(allocationResult);
		if (result)
		{
			mAllocators.at(selectedMemoryType.index).mAllocatedAddresses.emplace(result.Value().Address());
		}

		return result;
	}

	template<std::derived_from<MonoAllocator> Base>
	void NaivePolyAllocator<Base>::Free(MemoryBlock&& address) noexcept
	{
		for (auto& allocator: mAllocators)
		{
			if (allocator.second.mAllocatedAddresses.contains(address.Address()))
			{
				allocator.second.allocator.Free(std::move(address));
				break;
			}
		}

		Core::Unreachable();
	}
}
