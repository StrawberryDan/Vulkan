#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Allocator.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class NaiveAllocator
			: public Allocator
	{
	public:
		NaiveAllocator(Device& device);


		AllocationResult Allocate(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept override;
		void             Free(Allocation&& address) noexcept override;
	};
}
