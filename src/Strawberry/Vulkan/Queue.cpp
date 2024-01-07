//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Queue.hpp"
#include "Device.hpp"
#include "CommandBuffer.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Queue::Queue(const Device& device, uint32_t family, uint32_t index)
		: mFamilyIndex(family)
		, mDevice(device)
	{
		vkGetDeviceQueue(mDevice->mDevice, mFamilyIndex, index, &mQueue);
	}


	Queue::Queue(Queue&& rhs) noexcept
		: mQueue(std::exchange(rhs.mQueue, nullptr))
		, mFamilyIndex(std::exchange(rhs.mFamilyIndex, 0))
		, mDevice(std::move(rhs.mDevice))
	{}


	Queue& Queue::operator=(Queue&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Queue::~Queue()
	{
		if (mQueue)
		{
			WaitUntilIdle();
		}
	}


	void Queue::Submit(const CommandBuffer& commandBuffer)
	{
		WaitUntilIdle();

		VkSubmitInfo submitInfo {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer.mCommandBuffer,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr,
		};
		Core::AssertEQ(vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE), VK_SUCCESS);
	}


	void Queue::WaitUntilIdle()
	{
		vkQueueWaitIdle(mQueue);
	}


	Core::ReflexivePointer<Device> Queue::GetDevice() const
	{
		return mDevice;
	}


	uint32_t Queue::GetFamilyIndex() const
	{
		return mFamilyIndex;
	}
}
