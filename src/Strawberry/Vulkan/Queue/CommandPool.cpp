//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Queue/CommandPool.hpp"
#include "Strawberry/Vulkan/Device/Device.hpp"
#include "Strawberry/Vulkan/Queue/Queue.hpp"
// Strawberry Core
#include <Strawberry/Core/Assert.hpp>
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	CommandPool::CommandPool(Queue& queue, bool individualReset)
		: mQueue(queue)
	{
		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = individualReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : VkCommandPoolCreateFlags(0),
		};

		Core::Assert(vkCreateCommandPool(mQueue->GetDevice().Handle(), &createInfo, nullptr, &mCommandPool) == VK_SUCCESS);
	}


	CommandPool::CommandPool(CommandPool&& rhs) noexcept
		: mCommandPool(std::exchange(rhs.mCommandPool, nullptr))
		, mQueue(std::move(rhs.mQueue)) {}


	CommandPool& CommandPool::operator=(CommandPool&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	CommandPool::~CommandPool()
	{
		if (mCommandPool)
		{
			vkDestroyCommandPool(mQueue->GetDevice().Handle(), mCommandPool, nullptr);
		}
	}


	void CommandPool::Reset()
	{
		Core::AssertEQ(vkResetCommandPool(GetQueue()->GetDevice().Handle(), mCommandPool, 0), VK_SUCCESS);
	}


	Core::ReflexivePointer<Queue> CommandPool::GetQueue() const
	{
		return mQueue;
	}
}
