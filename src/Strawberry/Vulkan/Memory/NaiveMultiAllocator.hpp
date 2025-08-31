#pragma once
#include "Allocator.hpp"
#include <unordered_set>


namespace Strawberry::Vulkan
{
    template <std::derived_from<SingleAllocator> Base>
    class NaiveMultiAllocator
        : public MultiAllocator
    {
    public:
        explicit NaiveMultiAllocator(Device &device)
            : MultiAllocator(device)
        {}

        ~NaiveMultiAllocator() override = default;

        SingleAllocator& GetAllocator(MemoryTypeIndex typeIndex);

        AllocationResult Allocate(const AllocationRequest &allocationResult, const MemoryTypeCriteria &memoryTypeCriteria) noexcept override;

        void Free(Allocation &&address) noexcept override;

    private:
        struct BaseAllocator
        {
            Base allocator;
            std::unordered_set<Address> mAllocatedAddresses;
        };

        std::map<MemoryTypeIndex, BaseAllocator> mAllocators;
    };


    template<std::derived_from<SingleAllocator> Base>
    SingleAllocator& NaiveMultiAllocator<Base>::GetAllocator(MemoryTypeIndex typeIndex)
    {
        if (!mAllocators.contains(typeIndex)) [[unlikely]]
        {
            mAllocators.emplace(typeIndex, Base{ { GetDevice(), typeIndex, size_t{4 * 1024 * 1024}} });
        }

        return mAllocators.at(typeIndex).allocator;
    }

    template<std::derived_from<SingleAllocator> Base>
    AllocationResult NaiveMultiAllocator<Base>::Allocate(
        const AllocationRequest &allocationResult,
        const MemoryTypeCriteria &memoryTypeCriteria) noexcept
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

    template<std::derived_from<SingleAllocator> Base>
    void NaiveMultiAllocator<Base>::Free(Allocation&& address) noexcept
    {
        for (auto& allocator : mAllocators)
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
