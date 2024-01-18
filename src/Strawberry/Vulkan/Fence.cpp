//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Fence.hpp"
#include "Device.hpp"
// Standard Library
#include "Strawberry/Core/Assert.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Fence::Fence(const Device& device)
		: mFence(nullptr)
		, mDevice(device)
	{
		VkFenceCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
		};
		Core::AssertEQ(vkCreateFence(mDevice, &createInfo, nullptr, &mFence), VK_SUCCESS);
	}


	Fence::Fence(Fence&& rhs) noexcept
		: mFence(std::exchange(rhs.mFence, nullptr)), mDevice(std::exchange(rhs.mDevice, nullptr))
	{

	}


	Fence& Fence::operator=(Fence&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Fence::~Fence()
	{
		if (mFence)
		{
			vkDestroyFence(mDevice, mFence, nullptr);
		}
	}


	void Fence::Wait()
	{
		Core::AssertEQ(vkWaitForFences(mDevice, 1, &mFence, VK_TRUE, UINT64_MAX), VK_SUCCESS);
	}


	void Fence::Reset()
	{
		Core::AssertEQ(vkResetFences(mDevice, 1, &mFence), VK_SUCCESS);
	}
}
