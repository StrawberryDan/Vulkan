//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "CommandPool.hpp"
#include "Device.hpp"
#include "Queue.hpp"
// Strawberry Core
#include <Strawberry/Core/Assert.hpp>
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	CommandPool::CommandPool(const Queue& queue, bool resetBit)
		: mQueue(queue)
	{
		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = resetBit ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : VkCommandPoolCreateFlags(0),
		};

		Core::Assert(vkCreateCommandPool(*mQueue->GetDevice(), &createInfo, nullptr, &mCommandPool) == VK_SUCCESS);
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
			vkDestroyCommandPool(*mQueue->GetDevice(), mCommandPool, nullptr);
		}
	}


	Core::ReflexivePointer<Queue> CommandPool::GetQueue() const
	{
		return mQueue;
	}
}
