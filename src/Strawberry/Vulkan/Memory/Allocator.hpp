#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/Memory.hpp"
#include "Strawberry/Vulkan/Memory/AllocationRequest.hpp"
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
	class Allocator;
	class Allocation;


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


	// Class representing a block of allocated Vulkan Memory.
	class MemoryPool final
			: public Core::EnableReflexivePointer
	{
	public:
		static Core::Result<MemoryPool, AllocationError> Allocate(Device& device, MemoryTypeIndex memoryTypeIndex, size_t size);


		MemoryPool() = default;
		MemoryPool(Device& device, MemoryTypeIndex memoryTypeIndex, VkDeviceMemory memory, size_t size);
		MemoryPool(const MemoryPool&)            = delete;
		MemoryPool& operator=(const MemoryPool&) = delete;
		MemoryPool(MemoryPool&& other) noexcept;
		MemoryPool& operator=(MemoryPool&& other) noexcept;
		~MemoryPool() override;


		Allocation AllocateView(Allocator& allocator, size_t offset, size_t size);


		Core::ReflexivePointer<Device> GetDevice() const noexcept;
		VkDeviceMemory Memory() const noexcept;
		MemoryTypeIndex GetMemoryTypeIndex() const noexcept;
		size_t Size() const noexcept;


		VkMemoryPropertyFlags Properties() const;
		uint8_t*              GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Device>         mDevice          = nullptr;
		MemoryTypeIndex                        mMemoryTypeIndex = {};
		VkDeviceMemory                         mMemory          = VK_NULL_HANDLE;
		size_t                                 mSize            = 0;
		mutable Core::Optional<uint8_t*>       mMappedAddress   = Core::NullOpt;
	};


	using AllocationResult = Core::Result<Allocation, AllocationError>;


	class Allocator
			: public Core::EnableReflexivePointer
	{
	public:
		Allocator(Device& device);


		virtual void             Free(Allocation&& address) noexcept = 0;
		virtual                  ~Allocator() = default;



		Core::ReflexivePointer<Device> GetDevice() const { return mDevice; }


	private:
		Core::ReflexivePointer<Device> mDevice;
	};


	class SingleAllocator
		: public Allocator
	{
	public:
		SingleAllocator(Device& device, MemoryTypeIndex memoryTypeIndex)
			: Allocator(device)
			, mMemoryTypeIndex(memoryTypeIndex)
		{}

		virtual AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept = 0;


		const MemoryTypeIndex GetMemoryTypeIndex() const noexcept { return mMemoryTypeIndex; }


	private:
		MemoryTypeIndex mMemoryTypeIndex;
	};


	class PoolAllocator
		: public SingleAllocator
	{
	public:
		PoolAllocator(MemoryPool&& memoryPool)
			: SingleAllocator(*memoryPool.GetDevice(), memoryPool.GetMemoryTypeIndex())
			, mMemoryPool(std::move(memoryPool))
		{}


		const MemoryPool& Memory() const noexcept { return mMemoryPool; }
		      MemoryPool& Memory()       noexcept { return mMemoryPool; }


	private:
		MemoryPool mMemoryPool;
	};


	class MultiAllocator
		: public Allocator
	{
	public:
		using Allocator::Allocator;

		virtual AllocationResult Allocate(const AllocationRequest& allocationResult, const MemoryTypeCriteria& memoryTypeCriteria) noexcept = 0;
	};


	class Allocation
	{
	public:
		Allocation() = default;
		Allocation(const Device& device, Allocator& allocator, MemoryPool& allocation, size_t offset, size_t size);
		Allocation(const Allocation&)            = delete;
		Allocation& operator=(const Allocation&) = delete;
		Allocation(Allocation&& other) noexcept;
		Allocation& operator=(Allocation&& other) noexcept;
		~Allocation();


		explicit operator bool() const noexcept;


		[[nodiscard]] VkDevice                          GetDevice() const noexcept;
		[[nodiscard]] Core::ReflexivePointer<Allocator> GetAllocator() const noexcept;
		[[nodiscard]] Address                           Address() const noexcept;
		[[nodiscard]] VkDeviceMemory                    Memory() const noexcept;
		[[nodiscard]] size_t                            Offset() const noexcept;
		[[nodiscard]] size_t                            Size() const noexcept;
		[[nodiscard]] VkMemoryPropertyFlags             Properties() const;
		[[nodiscard]] uint8_t*                          GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;


	private:
		VkDevice                           mDevice     = VK_NULL_HANDLE;
		Core::ReflexivePointer<Allocator>  mAllocator  = nullptr;
		Core::ReflexivePointer<MemoryPool> mMemoryPool = nullptr;
		size_t                             mOffset     = 0;
		size_t                             mSize       = 0;
	};
}
