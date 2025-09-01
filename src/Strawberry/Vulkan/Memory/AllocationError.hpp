#pragma once
#include "Strawberry/Core/Types/Variant.hpp"

namespace Strawberry::Vulkan
{
    // Variant class representation an allocation error.
    class AllocationError
    {
    public:
        // Error for when the device could not find any memory to allocate.
        struct OutOfMemory {};

        // Error for when an allocator cannot service a request because it is larger than this allocator can support.
        struct InsufficientPoolSize {};


        template<typename T>
        AllocationError(T&& info)
            : mInfo(std::forward<T>(info)) {}


        template<typename T>
        [[nodiscard]] bool IsType() const noexcept
        {
            return mInfo.IsType<T>();
        }


        template <typename... Ts>
        [[nodiscard]] bool IsAnyOf() const noexcept
        {
            return (... || mInfo.IsType<Ts>());
        }


        template<typename T>
        [[nodiscard]] T GetInfo() const noexcept
        {
            Core::Assert(IsType<T>());
            return mInfo.Ref<T>();
        }


    private:
        using Info = Core::Variant<OutOfMemory, InsufficientPoolSize>;
        Info mInfo;
    };
}