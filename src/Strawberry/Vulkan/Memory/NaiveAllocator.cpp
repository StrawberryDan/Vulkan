//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "NaiveAllocator.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	NaiveAllocator::NaiveAllocator(Device& device)
		: Allocator(device) {}


	NaiveAllocator::RawAllocationResult NaiveAllocator::AllocateRaw(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept
	{
		auto physicalDevice       = GetDevice()->GetPhysicalDevices()[0];
		auto memoryTypeCandidates = physicalDevice->SearchMemoryTypes(typeMask, properties);


		if (memoryTypeCandidates.empty())
		{
			return RawAllocationResult::Err(AllocationError::MemoryTypeUnavailable());
		}
		auto chosenMemoryType = memoryTypeCandidates[0];


		VkMemoryAllocateInfo allocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = size,
			.memoryTypeIndex = chosenMemoryType,
		};

		Address address;
		VkResult   allocationResult = vkAllocateMemory(*GetDevice(), &allocateInfo, nullptr, &address.deviceMemory);

		switch (allocationResult)
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				return RawAllocationResult::Err(AllocationError::OutOfHostMemory());
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return RawAllocationResult::Err(AllocationError::OutOfDeviceMemory());
			case VK_SUCCESS:
				break;
			default:
				Core::Unreachable();
		}


		return Allocation(*this, address.deviceMemory, size, physicalDevice->GetMemoryProperties().memoryTypes[chosenMemoryType].propertyFlags);
	}


	AllocationResult NaiveAllocator::Allocate(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept
	{
		if (auto allocation = AllocateRaw(size, typeMask, properties))
		{
			auto address = allocation->Memory();
			auto allocationIter = mAllocations.emplace(address, allocation.Unwrap()).first;
			return allocationIter->second.AllocateView(0, size);
		}
		else
		{
			return allocation.Err();
		}
	}


	void NaiveAllocator::Free(AllocationView&& address) noexcept
	{
		// Free memory
		mAllocations.erase(address.Memory());
	}
}
