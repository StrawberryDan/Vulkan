#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Address.hpp"
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


	using AllocationResult = Core::Result<Allocation, AllocationError>;


	class Allocator
			: public Core::EnableReflexivePointer
	{
	public:
		Allocator(Device& device);


		virtual AllocationResult Allocate(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept = 0;
		virtual void             Free(GPUAddress address) noexcept = 0;
		virtual                  ~Allocator() = default;


		Core::ReflexivePointer<Device> GetDevice() const noexcept;


		VkMemoryPropertyFlags GetMemoryProperties(VkDeviceMemory memory) const noexcept;
		bool                  MemoryHasProperty(VkDeviceMemory memory, VkMemoryPropertyFlags properties) const noexcept;


		Core::Optional<uint8_t*> MapMemory(VkDeviceMemory memory) const noexcept;
		void                     UnmapMemory(VkDeviceMemory memory) const noexcept;
		bool                     IsMemoryMapped(VkDeviceMemory memory) const noexcept;
		void                     FlushMappedMemory(VkDeviceMemory memory) const noexcept;

	protected:
		// Deriving classes MUST keep this table up to date in their Allocate and Free methods.
		mutable std::map<VkDeviceMemory, VkMemoryPropertyFlags> mMemoryProperties;

	private:
		Core::ReflexivePointer<Device>             mDevice;
		mutable std::map<VkDeviceMemory, uint8_t*> mMemoryMappings;
	};


	class Allocation
	{
	public:
		Allocation() = default;
		Allocation(Allocator& allocator, GPUMemoryRange memoryRange);
		Allocation(const Allocation&)            = delete;
		Allocation& operator=(const Allocation&) = delete;
		Allocation(Allocation&& other) noexcept;
		Allocation& operator=(Allocation&& other) noexcept;
		~Allocation();


		const GPUAddress& Address() const noexcept;
		const size_t      Size() const noexcept;


		uint8_t* GetMappedAddress() const noexcept;
		void     Flush() const noexcept;
		void     Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Allocator> mAllocator = nullptr;
		GPUMemoryRange                    mRange     = GPUMemoryRange();
	};
}
