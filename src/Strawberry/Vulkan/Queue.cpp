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
	Queue::Queue(Device& device, uint32_t family, uint32_t index)
		: mFamilyIndex(family)
		, mDevice(device)
	{
		vkGetDeviceQueue(*mDevice, mFamilyIndex, index, &mQueue);
	}


	Queue::Queue(Queue&& rhs) noexcept
		: mQueue(std::exchange(rhs.mQueue, nullptr))
		, mFamilyIndex(std::exchange(rhs.mFamilyIndex, 0))
		, mDevice(std::move(rhs.mDevice)) {}


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
		Core::AssertEQ(commandBuffer.Level(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		WaitUntilIdle();

		VkCommandBuffer handle = commandBuffer;
		VkSubmitInfo    submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = &handle,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr,
		};

		commandBuffer.mExecutionFenceOrParentBuffer.Ptr<Fence>()->Reset();
		commandBuffer.MoveIntoPendingState();
		Core::AssertEQ(vkQueueSubmit(mQueue, 1, &submitInfo, commandBuffer.mExecutionFenceOrParentBuffer.Ptr<Fence>()->mFence), VK_SUCCESS);
	}


	void Queue::WaitUntilIdle() const
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
