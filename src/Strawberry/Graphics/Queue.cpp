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
namespace Strawberry::Graphics
{
	Queue::Queue(const Device& device)
		: mDevice(device.mDevice)
	{
		vkGetDeviceQueue(mDevice, device.mQueueFamilyIndex, 0, &mQueue);

		VkFenceCreateInfo fenceCreateInfo {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		Core::AssertEQ(vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mSubmissionFence), VK_SUCCESS);
	}


	Queue::Queue(Queue&& rhs) noexcept
		: mQueue(std::exchange(rhs.mQueue, nullptr))
		, mSubmissionFence(std::exchange(rhs.mSubmissionFence, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
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
			vkDestroyFence(mDevice, mSubmissionFence, nullptr);
		}
	}


	void Queue::Submit(const CommandBuffer& commandBuffer)
	{
		VkSubmitInfo submitInfo {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = 0,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer.mCommandBuffer,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr
		};

		vkQueueSubmit(mQueue, 1, &submitInfo, mSubmissionFence);
		vkWaitForFences(mDevice, 1, &mSubmissionFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(mDevice, 1, &mSubmissionFence);
	}
}
