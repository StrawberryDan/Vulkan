#pragma once
// Includes
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/Allocator.hpp"
#include "Strawberry/Vulkan/Memory/BuddyAllocator.hpp"
#include "Strawberry/Vulkan/Memory/FreelistAllocator.hpp"
// Standard Library
#include <concepts>
#include <unordered_set>



namespace Strawberry::Vulkan
{
    template <typename T>
    concept MemoryPolicy = requires(T t, AllocationRequest allocationRequest)
    {
        { t.Allocate(allocationRequest) } -> std::same_as<AllocationResult>;
        { t.Free(std::declval<Allocation&&>()) }; 
    };

    namespace MemoryPolicies
    {
        struct Refuse
        {
            AllocationResult Allocate(const AllocationRequest& request) noexcept
            {
                return AllocationError::OutOfMemory(); 
            }

            void Free(Allocation&& allocation) noexcept 
            {
                Core::Unreachable();
            }
        };

        struct Naive 
        {
            Naive(const PhysicalDevice& physicalDevice, Allocator* allocator, uint32_t memoryTypeIndex)
                : mPhysicalDevice(physicalDevice)
                , mAllocator(allocator)
                , mMemoryTypeIndex(memoryTypeIndex)
            {}

            AllocationResult Allocate(const AllocationRequest& request) noexcept
            {
                auto allocation = MemoryPool::Allocate(*request.device, *mPhysicalDevice, mMemoryTypeIndex, request.size);

                if (allocation.IsOk())
                {
                    mMemoryPools.emplace_back(allocation.Unwrap());
                    return mMemoryPools.back().AllocateView(*mAllocator, 0, request.size);
                }
            }

            Core::ReflexivePointer<PhysicalDevice> mPhysicalDevice;
            Allocator* mAllocator;
            uint32_t mMemoryTypeIndex;
            std::vector<MemoryPool> mMemoryPools;
        };

        template <std::derived_from<Allocator> Base>
        struct Single
        {
        public:
            template <typename... Args> requires std::constructible_from<Base, Args...>
            Single(Args&&... args)
                : mBase(std::forward<Args>(args)...)
            {}

            Single(Base&& base)
                : mBase(std::move(base))
            {}

            AllocationResult Allocate(const AllocationRequest& request) noexcept
            {
                return mBase.Allocate(request);
            }

            void Free(Allocation&& allocation) noexcept
            {
                mBase.Free(std::move(allocation));
            }


        private:
            Base mBase;
        };
    

        template <std::derived_from<Allocator> Base>
        struct Stack
        {
        public:
            using CreateBlockFn = std::function<Base()>;

            template <typename F> requires std::same_as<std::invoke_result_t<F>, Base>
            Stack(F&& createBlockFn)
                : mNewBlockFunction(std::move(createBlockFn))
            {
                mAllocators.emplace_back(mNewBlockFunction());
            }


            AllocationResult Allocate(this auto& self, const AllocationRequest& request) noexcept
            {
                for (auto& allocator : self.mAllocators)
                {
                    auto allocation = allocator.Allocate(request);
                    
                    if (allocation.IsOk() || !allocation.Err().template IsType<AllocationError::OutOfMemory>())
                    {
                        return allocation;
                    }
                }

                self.mAllocators.emplace_back(self.mNewBlockFunction());
                return self.mAllocators.back().Allocate(request);
            }

            void Free(Allocation&& allocation) noexcept
            {
                for (auto& allocator : mAllocators)
                {
                    if (allocator.Memory().Memory() == allocation.Memory())
                    {
                        allocator.Free(std::move(allocation));
                    }
                }

                Core::Unreachable();
            }


        private:
            CreateBlockFn mNewBlockFunction;
            std::vector<Base> mAllocators;
        };
    }

    template <typename T>
    concept ComplexAllocatorDescription = requires(T t)
    {
        requires MemoryPolicy<typename T::MemoryPolicy>;
        requires MemoryPolicy<typename T::LargeRequestPolicy>;
    };

    template <ComplexAllocatorDescription Description>
    class PolicyAllocator
        : public Allocator
    {
    public:
        PolicyAllocator(
                Description::MemoryPolicy memoryPolicy, 
                Description::LargeRequestPolicy largeRequestPolicy
            )
            : mMemoryPolicy(std::move(memoryPolicy))
            , mLargeRequestPolicy(std::move(largeRequestPolicy))
        {}


        AllocationResult Allocate(const AllocationRequest& request) noexcept override
        {
            AllocationResult allocation = mMemoryPolicy.Allocate(request);

            const bool suitableForLargeAllocation = allocation.IsErr() &&
                allocation.Err().IsType<AllocationError::RequestTooLarge>();
            if (suitableForLargeAllocation)
            {
                allocation = mLargeRequestPolicy.Allocate(request);

                if (allocation.IsOk())
                {
                    mLargeAllocations.emplace(allocation.Value().Address());
                }
            }

            return allocation;
        }

        void Free(Allocation&& allocation) noexcept override
        {
            if (mLargeAllocations.contains(allocation.Address()))
            {
                mLargeAllocations.erase(allocation.Address());
                mLargeRequestPolicy.Free(std::move(allocation));
            }
            else
            {
                mMemoryPolicy.Free(std::move(allocation));
            }
        }

        


    private:
        Description::MemoryPolicy mMemoryPolicy;
        Description::LargeRequestPolicy mLargeRequestPolicy;
        std::unordered_set<Address> mLargeAllocations;
    };


    struct BasicAllocatorDescription 
    {
        using MemoryPolicy = MemoryPolicies::Stack<FreeListAllocator>;
        using LargeRequestPolicy = MemoryPolicies::Refuse;
    };

    class BasicAllocator
        : public PolicyAllocator<BasicAllocatorDescription>
    {
    public:
        BasicAllocator(const Device& device, const PhysicalDevice& physicalDevice, uint32_t memoryTypeIndex)
            : PolicyAllocator(
                MemoryPolicies::Stack<FreeListAllocator>(
                    [device = device.GetReflexivePointer(), physicalDevice = physicalDevice.GetReflexivePointer(), memoryTypeIndex]()
                        { return FreeListAllocator(MemoryPool::Allocate(*device, *physicalDevice, memoryTypeIndex, 1024 * 1024 * 128).Unwrap()); } ),
                MemoryPolicies::Refuse())
            , mDevice(device)
            , mPhysicalDevice(physicalDevice)
            , mMemoryTypeIndex(memoryTypeIndex)
        {}


    private:
        Core::ReflexivePointer<Device> mDevice;
        Core::ReflexivePointer<PhysicalDevice> mPhysicalDevice;
        uint32_t mMemoryTypeIndex;
    };
}