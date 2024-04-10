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
		auto physicalDevice = GetDevice()->GetPhysicalDevices()[0];
		auto memoryTypeCandidates = physicalDevice->SearchMemoryTypes(typeMask, properties);
		Core::Assert(!memoryTypeCandidates.empty());
		auto chosenMemoryType = memoryTypeCandidates[0];


		VkMemoryAllocateInfo allocateInfo
		{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.pNext = nullptr,
				.allocationSize = size,
				.memoryTypeIndex = chosenMemoryType,
		};

		GPUAddress address;
		Core::AssertEQ(vkAllocateMemory(*GetDevice(), &allocateInfo, nullptr, &address.deviceMemory), VK_SUCCESS);

		mMemoryProperties.emplace(address.deviceMemory, physicalDevice->GetMemoryProperties().memoryTypes[chosenMemoryType].propertyFlags);

		return Allocation(*this, {address, size});
	}


	void NaiveAllocator::Free(GPUAddress address) noexcept
	{
		mMemoryProperties.erase(address.deviceMemory);
		vkFreeMemory(*GetDevice(), address.deviceMemory, nullptr);
	}
}