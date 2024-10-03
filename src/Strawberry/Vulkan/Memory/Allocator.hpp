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
	class Allocator;
	class Allocation;


	class AllocationError
	{
	public:
		struct OutOfMemory {};


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
		using Info = Core::Variant<OutOfMemory, MemoryTypeUnavailable>;
		Info mInfo;
	};


	class MemoryPool final
			: public Core::EnableReflexivePointer
	{
	public:
		static Core::Result<MemoryPool, AllocationError> Allocate(Device& device, const PhysicalDevice& physicalDevice, uint32_t memoryTypeIndex, size_t size);


		MemoryPool() = default;
		MemoryPool(Device& device, const PhysicalDevice& physicalDevice, uint32_t memoryTypeIndex, VkDeviceMemory memory, size_t size);
		MemoryPool(const MemoryPool&)            = delete;
		MemoryPool& operator=(const MemoryPool&) = delete;
		MemoryPool(MemoryPool&& other) noexcept;
		MemoryPool& operator=(MemoryPool&& other) noexcept;
		~MemoryPool() override;


		Allocation AllocateView(Allocator& allocator, size_t offset, size_t size);


		VkDeviceMemory Memory() const noexcept
		{
			return mMemory;
		}


		uint32_t MemoryTypeIndex() const noexcept
		{
			return mMemoryTypeIndex;
		}


		size_t Size() const noexcept
		{
			return mSize;
		}


		VkMemoryPropertyFlags Properties() const;
		uint8_t*              GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Device>         mDevice          = nullptr;
		Core::ReflexivePointer<PhysicalDevice> mPhysicalDevice  = nullptr;
		uint32_t                               mMemoryTypeIndex = -1;
		VkDeviceMemory                         mMemory          = VK_NULL_HANDLE;
		size_t                                 mSize            = 0;
		mutable Core::Optional<uint8_t*>       mMappedAddress   = Core::NullOpt;
	};


	using AllocationResult = Core::Result<Allocation, AllocationError>;


	struct AllocationRequest
	{
		AllocationRequest(size_t size, size_t alignment)
			: size(size)
			, alignment(alignment) {}


		AllocationRequest(VkMemoryRequirements& requirements);

		size_t size;
		size_t alignment;
	};


	class Allocator
			: public Core::EnableReflexivePointer
	{
	public:
		explicit Allocator(Device& device, MemoryPool&& memoryPool);


		virtual AllocationResult Allocate(const AllocationRequest& allocationRequest) noexcept = 0;
		void                     Free(MemoryPool&& allocation) const;
		virtual void             Free(Allocation&& address) noexcept = 0;
		virtual                  ~Allocator() = default;


		[[nodiscard]] Core::ReflexivePointer<Device> GetDevice() const noexcept;


		[[nodiscard]] uint32_t MemoryTypeIndex() const noexcept
		{
			return mMemoryPool.MemoryTypeIndex();
		}

	protected:
		MemoryPool& GetMemoryPool()
		{
			return mMemoryPool;
		}

	private:
		Core::ReflexivePointer<Device> mDevice;
		MemoryPool                     mMemoryPool;
	};


	class Allocation
	{
	public:
		Allocation() = default;
		Allocation(Allocator& allocator, MemoryPool& allocation, size_t offset, size_t size);
		Allocation(const Allocation&)            = delete;
		Allocation& operator=(const Allocation&) = delete;
		Allocation(Allocation&& other) noexcept;
		Allocation& operator=(Allocation&& other) noexcept;
		~Allocation();


		explicit operator bool() const noexcept
		{
			Core::AssertImplication(!mAllocator, mRawAllocation);
			return mAllocator;
		}


		[[nodiscard]] Core::ReflexivePointer<Allocator> GetAllocator() const noexcept;
		[[nodiscard]] VkDeviceMemory                    Memory() const noexcept;
		[[nodiscard]] size_t                            Offset() const noexcept;
		[[nodiscard]] size_t                            Size() const noexcept;
		[[nodiscard]] VkMemoryPropertyFlags             Properties() const;
		[[nodiscard]] uint8_t*                          GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Allocator>  mAllocator     = nullptr;
		Core::ReflexivePointer<MemoryPool> mRawAllocation = nullptr;
		size_t                             mOffset        = 0;
		size_t                             mSize          = 0;
	};
}
