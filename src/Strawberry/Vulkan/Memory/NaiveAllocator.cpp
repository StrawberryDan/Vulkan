//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "NaiveAllocator.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	NaiveAllocator::NaiveAllocator(Device& device, uint32_t memoryType)
		: Allocator(device, memoryType) {}


	NaiveAllocator::RawAllocationResult NaiveAllocator::AllocateRaw(size_t size) noexcept
	{
		const auto physicalDevice       = GetDevice()->GetPhysicalDevices()[0];


		const VkMemoryAllocateInfo allocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = size,
			.memoryTypeIndex = MemoryType(),
		};

		Address  address;

		switch (VkResult allocationResult = vkAllocateMemory(*GetDevice(), &allocateInfo, nullptr, &address.deviceMemory))
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return RawAllocationResult::Err(AllocationError::OutOfMemory());
			case VK_SUCCESS:
				break;
			default:
				Core::Unreachable();
		}


		return MemoryPool(*GetDevice(), *physicalDevice, MemoryType(), address.deviceMemory, size);
	}


	AllocationResult NaiveAllocator::Allocate(const AllocationRequest& allocationRequest) noexcept
	{
		if (auto allocation = AllocateRaw(allocationRequest.size))
		{
			auto address        = allocation->Memory();
			auto allocationIter = mAllocations.emplace(address, allocation.Unwrap()).first;
			return allocationIter->second.AllocateView(*this, 0, allocationRequest.size);
		}
		else
		{
			return allocation.Err();
		}
	}


	void NaiveAllocator::Free(Allocation&& address) noexcept
	{
		// Free memory
		mAllocations.erase(address.Memory());
	}
}
