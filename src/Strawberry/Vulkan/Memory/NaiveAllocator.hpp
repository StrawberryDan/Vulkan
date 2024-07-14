#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Allocator.hpp"
// Standard Library
#include <set>


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
		void             Free(AllocationView&& address) noexcept override;

	private:
		std::map<VkDeviceMemory, Allocation> mAllocations;
	};
}
