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


	AllocationResult NaiveAllocator::Allocate(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept
	{
		auto physicalDevice       = GetDevice()->GetPhysicalDevices()[0];
		auto memoryTypeCandidates = physicalDevice->SearchMemoryTypes(typeMask, properties);


		if (memoryTypeCandidates.empty())
		{
			return AllocationResult::Err(AllocationError::MemoryTypeUnavailable());
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
				return AllocationResult::Err(AllocationError::OutOfHostMemory());
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return AllocationResult::Err(AllocationError::OutOfDeviceMemory());
			case VK_SUCCESS:
				break;
			default:
				Core::Unreachable();
		}


		Allocation allocation(*this, address.deviceMemory, size, physicalDevice->GetMemoryProperties().memoryTypes[chosenMemoryType].propertyFlags);
		auto allocationIter = mAllocations.emplace(address.deviceMemory, std::move(allocation)).first;

		return allocationIter->second.AllocateView(0, size);
	}


	void NaiveAllocator::Free(AllocationView&& address) noexcept
	{
		// Free memory
		mAllocations.erase(address.Memory());
	}
}
