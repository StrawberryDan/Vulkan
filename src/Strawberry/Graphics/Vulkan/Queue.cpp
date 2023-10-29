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
namespace Strawberry::Graphics::Vulkan
{
	Queue::Queue(const Device& device)
		: mFamilyIndex(device.mQueueFamilyIndex)
		, mDevice(device)
		, mSubmissionFence(device)
	{
		vkGetDeviceQueue(mDevice->mDevice, mFamilyIndex, 0, &mQueue);

		mCommandPool.Construct<const Queue&>(*this, true);
	}


	Queue::Queue(Queue&& rhs) noexcept
		: mQueue(std::exchange(rhs.mQueue, nullptr))
		, mDevice(std::move(rhs.mDevice))
		, mSubmissionFence(std::move(rhs.mSubmissionFence))
	{
		mCommandPool.Construct(std::move(rhs.mCommandPool.Get()));
	}


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
		mCommandPool.Destruct();
	}


	void Queue::Submit(const CommandBuffer& commandBuffer)
	{
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
		Core::AssertEQ(vkQueueSubmit(mQueue, 1, &submitInfo, mSubmissionFence.mFence), VK_SUCCESS);

		mSubmissionFence.Wait();
		mSubmissionFence.Reset();
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
