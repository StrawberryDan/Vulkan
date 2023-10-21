//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include "ImageView.hpp"
#include "Pipeline.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "ImageView.hpp"
// Standard Library
#include <memory>
#include <Strawberry/Core/Math/Vector.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	CommandBuffer::CommandBuffer(const CommandPool& commandPool)
		: mCommandBuffer{}
		  , mDevice(commandPool.mDevice)
		  , mCommandPool(commandPool.mCommandPool)
		  , mQueueFamilyIndex(commandPool.mQueueFamilyIndex)
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
		  , mDevice(std::exchange(rhs.mDevice, nullptr))
		  , mQueueFamilyIndex(std::exchange(rhs.mQueueFamilyIndex, 0))
	{

	}


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


	void CommandBuffer::Begin(bool oneTimeSubmit)
	{
		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VkCommandBufferUsageFlags(0),
			.pInheritanceInfo = nullptr,
		};

		Core::AssertEQ(vkBeginCommandBuffer(mCommandBuffer, &beginInfo), VK_SUCCESS);
	}


	void CommandBuffer::End()
	{
		Core::AssertEQ(vkEndCommandBuffer(mCommandBuffer), VK_SUCCESS);
	}


	void CommandBuffer::Reset()
	{
		Core::AssertEQ(vkResetCommandBuffer(mCommandBuffer, 0), VK_SUCCESS);
	}


	void CommandBuffer::BindPipeline(const Pipeline& pipeline)
	{
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mPipeline);
	}


	void CommandBuffer::BindVertexBuffer(uint32_t binding, Buffer& buffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(mCommandBuffer, binding, 1, &buffer.mBuffer, &offset);
	}


	void
	CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset)
	{
		vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}


	void CommandBuffer::BeginRenderPass(ImageView& colorAttachment)
	{
		VkRenderingAttachmentInfo colorAttachmentInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = colorAttachment.mImageView,
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue{.color{.uint32{0, 0, 0, 0}}}
		};

		VkRenderingInfo renderingInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderArea{
				.offset {0, 0},
				.extent = {static_cast<uint32_t>(colorAttachment.mSize[0]), static_cast<uint32_t>(colorAttachment.mSize[1])},
			},
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
			.pDepthAttachment = nullptr,
			.pStencilAttachment = nullptr,
		};

		vkCmdBeginRendering(mCommandBuffer, &renderingInfo);
	}


	void CommandBuffer::EndRenderPass()
	{
		vkCmdEndRendering(mCommandBuffer);
	}


}
