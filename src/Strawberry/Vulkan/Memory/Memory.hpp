#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	struct MemoryTypeIndex
	{
		VkPhysicalDevice physicalDevice;
		unsigned         memoryTypeIndex;


		VkMemoryPropertyFlags GetProperties() const noexcept;
	};


	struct MemoryType
	{
		MemoryTypeIndex       index;
		size_t                heapSize;
		VkMemoryPropertyFlags properties;
	};


	struct Address
	{
		VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
		uintptr_t      offset       = 0;


		bool operator==(const Address&) const noexcept = default;
		bool operator!=(const Address&) const noexcept = default;
	};


	struct MemorySpan
	{
		Address address = Address();
		size_t  size    = 0;
	};
}


template<>
struct std::hash<Strawberry::Vulkan::Address>
{
	std::size_t operator()(const Strawberry::Vulkan::Address& address) const noexcept
	{
		std::size_t hash = reinterpret_cast<uintptr_t>(address.deviceMemory);
		hash             = hash xor address.offset;
		return hash;
	}
};
