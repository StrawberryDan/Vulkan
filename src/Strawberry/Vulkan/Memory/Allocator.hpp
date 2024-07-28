#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/Memory.hpp"
#include "Strawberry/Vulkan/Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Variant.hpp"
#include "Strawberry/Core/Types/Result.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <unordered_map>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------


namespace Strawberry::Vulkan
{
	class AllocationError
	{
	public:
		struct OutOfHostMemory {};


		struct OutOfDeviceMemory {};


		struct MemoryTypeUnavailable {};


		template<typename T>
		AllocationError(T info)
			: mInfo(info) {}


		template<typename T>
		[[nodiscard]] bool IsType() const noexcept
		{
			return mInfo.IsType<T>();
		}


		template<typename T>
		[[nodiscard]] T GetInfo() const noexcept
		{
			Core::Assert(IsType<T>());
			return mInfo.Value<T>();
		}

	private:
		using Info = Core::Variant<OutOfHostMemory, OutOfDeviceMemory, MemoryTypeUnavailable>;
		Info mInfo;
	};


	class Allocation;
	class AllocationView;


	using AllocationResult = Core::Result<AllocationView, AllocationError>;


	class Allocator
			: public Core::EnableReflexivePointer
	{
	public:
		explicit Allocator(Device& device);


		virtual AllocationResult Allocate(size_t size, const MemoryTypeCriteria& criteria) noexcept = 0;
		void                     Free(Allocation&& allocation) const;
		virtual void             Free(AllocationView&& address) noexcept = 0;
		virtual                  ~Allocator() = default;


		[[nodiscard]] Core::ReflexivePointer<Device> GetDevice() const noexcept;

	private:
		Core::ReflexivePointer<Device> mDevice;
	};


	class Allocation
			: public Core::EnableReflexivePointer
	{
	public:
		Allocation() = default;
		Allocation(Allocator& allocator, VkDeviceMemory memory, size_t size, VkMemoryPropertyFlags memoryType);
		Allocation(const Allocation&)            = delete;
		Allocation& operator=(const Allocation&) = delete;
		Allocation(Allocation&& other) noexcept;
		Allocation& operator=(Allocation&& other) noexcept;
		~Allocation() override;


		AllocationView AllocateView(size_t offset, size_t size);


		Core::ReflexivePointer<Allocator> GetAllocator() const noexcept;
		VkDeviceMemory                    Memory() const noexcept;
		size_t                            Size() const noexcept;
		VkMemoryPropertyFlags             Properties() const;
		uint8_t*                          GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Allocator> mAllocator        = nullptr;
		VkDeviceMemory                    mMemory           = VK_NULL_HANDLE;
		size_t                            mSize             = 0;
		VkMemoryPropertyFlags             mMemoryProperties = 0;
		mutable Core::Optional<uint8_t*>  mMappedAddress;
	};


	class AllocationView
	{
	public:
		AllocationView() = default;
		AllocationView(Allocation& allocation, size_t offset, size_t size);


		[[nodiscard]] Core::ReflexivePointer<Allocator> GetAllocator() const noexcept;
		[[nodiscard]] VkDeviceMemory                    Memory() const noexcept;
		[[nodiscard]] size_t                            Offset() const noexcept;
		[[nodiscard]] size_t                            Size() const noexcept;
		[[nodiscard]] VkMemoryPropertyFlags             Properties() const;
		[[nodiscard]] uint8_t*                          GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Allocation> mAllocation = nullptr;
		size_t                             mOffset     = 0;
		size_t                             mSize       = 0;
	};
}
