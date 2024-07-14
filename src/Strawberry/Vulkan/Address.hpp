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
	struct GPUAddress
	{
		VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
		uintptr_t      offset       = 0;


		bool operator==(const GPUAddress&) const noexcept = default;
		bool operator!=(const GPUAddress&) const noexcept = default;
	};


	struct GPUMemoryRange
	{
		GPUAddress address = GPUAddress();
		size_t     size    = 0;
	};
}


namespace std
{
	template <>
	struct hash<Strawberry::Vulkan::GPUAddress>
	{
		std::size_t operator()(const Strawberry::Vulkan::GPUAddress& address) const noexcept
		{
			std::size_t hash = reinterpret_cast<uintptr_t>(address.deviceMemory);
			hash = hash xor address.offset;
			return hash;
		}
	};
}