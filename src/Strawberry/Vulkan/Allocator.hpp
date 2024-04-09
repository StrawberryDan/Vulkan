#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
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
	struct GPUAddress
	{
		VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
		size_t         offset       = 0;


		bool operator==(const GPUAddress&) const noexcept = default;
		bool operator!=(const GPUAddress&) const noexcept = default;
	};
}


namespace std
{
	template <>
	struct hash<Strawberry::Vulkan::GPUAddress>
	{
		std::size_t operator()(const Strawberry::Vulkan::GPUAddress& address) const noexcept
		{
			std::size_t hash = reinterpret_cast<std::size_t>(address.deviceMemory);
			hash = hash xor address.offset;
			return hash;
		}
	};
}


namespace Strawberry::Vulkan
{
	struct GPUMemoryRange
	{
		GPUAddress address = GPUAddress();
		size_t     size    = 0;
	};


	struct AllocationError
	{

	};


	class Allocation;


	using AllocationResult = Core::Result<Allocation, AllocationError>;


	class Allocator
		: public Core::EnableReflexivePointer<Allocator>
	{
	public:
		                         Allocator(Device& device);


		virtual AllocationResult Allocate(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept = 0;
		virtual void             Free    (GPUAddress address) noexcept = 0;
		virtual                 ~Allocator() = default;


		Core::ReflexivePointer<Device>   GetDevice() const noexcept;


		VkMemoryPropertyFlags    GetMemoryProperties(VkDeviceMemory memory) const noexcept;
		bool                     MemoryHasProperty(VkDeviceMemory memory, VkMemoryPropertyFlags properties) const noexcept;


		Core::Optional<uint8_t*> MapMemory        (VkDeviceMemory memory) const noexcept;
		void                     UnmapMemory      (VkDeviceMemory memory) const noexcept;
		bool                     IsMemoryMapped   (VkDeviceMemory memory) const noexcept;
		void                     FlushMappedMemory(VkDeviceMemory memory) const noexcept;


	protected:
		// Deriving classes MUST keep this table up to date in their Allocate and Free methods.
		mutable std::map<VkDeviceMemory, VkMemoryPropertyFlags> mMemoryProperties;


	private:
		Core::ReflexivePointer<Device>                          mDevice;
		mutable std::map<VkDeviceMemory, uint8_t*>              mMemoryMappings;
	};


	class Allocation
	{
	public:
		Allocation() = default;
		Allocation(Allocator& allocator, GPUMemoryRange memoryRange);
		Allocation(const Allocation&) = delete;
		Allocation& operator=(const Allocation&) = delete;
		Allocation(Allocation&& other) noexcept;
		Allocation& operator=(Allocation&& other) noexcept;
		~Allocation();


		const GPUAddress& Address() const noexcept;
		const size_t      Size()    const noexcept;


		uint8_t*          GetMappedAddress() const noexcept;
		void              Flush() const noexcept;
		void              Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;


	private:
		Core::ReflexivePointer<Allocator> mAllocator = nullptr;
		GPUMemoryRange                    mRange     = GPUMemoryRange();
	};
}
