//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	CommandBuffer::CommandBuffer(const CommandPool& commandPool)
		: mCommandBuffer{}
		, mDevice(commandPool.mDevice)
		  , mCommandPool(commandPool.mCommandPool)
	{
		VkCommandBufferAllocateInfo allocateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		Core::Assert(vkAllocateCommandBuffers(mDevice, &allocateInfo, &mCommandBuffer) == VK_SUCCESS);
	}


	CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept
		: mCommandBuffer(std::exchange(rhs.mCommandBuffer, nullptr))
		  , mCommandPool(std::exchange(rhs.mCommandPool, nullptr))
		  , mDevice(std::exchange(rhs.mDevice, nullptr)) {}


	CommandBuffer& CommandBuffer::operator=(CommandBuffer&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	CommandBuffer::~CommandBuffer()
	{
		if (mCommandBuffer)
		{
			vkFreeCommandBuffers(mDevice, mCommandPool, 1, &mCommandBuffer);
		}
	}
}
