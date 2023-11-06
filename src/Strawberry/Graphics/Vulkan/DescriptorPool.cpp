//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "DescriptorPool.hpp"
#include "Device.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	DescriptorPool::DescriptorPool(const Device& device, VkDescriptorPoolCreateFlags flags, uint32_t maxSets, std::vector<VkDescriptorPoolSize> poolSizes)
		: mDescriptorPool(VK_NULL_HANDLE)
		, mDevice(device)
	{
		VkDescriptorPoolCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.maxSets = maxSets,
			.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
			.pPoolSizes = poolSizes.data(),
		};

		Core::AssertEQ(vkCreateDescriptorPool(mDevice->mDevice, &createInfo, nullptr, &mDescriptorPool),
					   VK_SUCCESS);
	}


	DescriptorPool::DescriptorPool(DescriptorPool&& rhs) noexcept
		: mDescriptorPool(std::exchange(rhs.mDescriptorPool, VK_NULL_HANDLE))
		, mDevice(std::move(rhs.mDevice))
	{}


	DescriptorPool& DescriptorPool::operator=(DescriptorPool&& rhs) noexcept
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	DescriptorPool::~DescriptorPool()
	{
		if (mDescriptorPool)
		{
			vkDestroyDescriptorPool(mDevice->mDevice, std::exchange(mDescriptorPool, VK_NULL_HANDLE), nullptr);
		}
	}


	Core::ReflexivePointer<Device> DescriptorPool::GetDevice() const
	{
		return mDevice;
	}
}
