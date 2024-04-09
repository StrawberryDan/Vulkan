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
			: Allocator(device)
	{}


	AllocationResult NaiveAllocator::Allocate(size_t size, uint32_t typeMask, VkMemoryPropertyFlags properties) noexcept
	{
		auto memoryTypeCandidates = GetDevice()->GetPhysicalDevices()[0]->SearchMemoryTypes(typeMask, properties);
		Core::Assert(!memoryTypeCandidates.empty());


		VkMemoryAllocateInfo allocateInfo
				{
						.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
						.pNext = nullptr,
						.allocationSize = size,
						.memoryTypeIndex = memoryTypeCandidates[0]
				};

		GPUAddress address;
		Core::AssertEQ(vkAllocateMemory(*GetDevice(), &allocateInfo, nullptr, &address.deviceMemory), VK_SUCCESS);

		mMemoryProperties.emplace(address.deviceMemory, properties);

		return Allocation(*this, {address, size});
	}


	void NaiveAllocator::Free(GPUAddress address) noexcept
	{
		mMemoryProperties.erase(address.deviceMemory);
		vkFreeMemory(*GetDevice(), address.deviceMemory, nullptr);
	}
}