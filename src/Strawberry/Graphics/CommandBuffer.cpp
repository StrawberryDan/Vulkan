//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include "ImageView.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"
#include "Framebuffer.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
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


	void CommandBuffer::ImageMemoryBarrier(const Image& image, VkImageAspectFlagBits aspect, VkImageLayout targetLayout)
	{
		VkImageMemoryBarrier imageMemoryBarrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_NONE,
			.dstAccessMask = VK_ACCESS_NONE,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = targetLayout,
			.srcQueueFamilyIndex = mQueueFamilyIndex,
			.dstQueueFamilyIndex = mQueueFamilyIndex,
			.image = image.mImage,
			.subresourceRange{
				.aspectMask = aspect,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		vkCmdPipelineBarrier(mCommandBuffer,
							 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
							 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
							 0,
							 0, nullptr, 0, nullptr, 1,
							 &imageMemoryBarrier);
	}


	void CommandBuffer::BeginRenderPass(const Pipeline& pipeline, const Framebuffer& framebuffer)
	{
		VkClearValue clearValue{
			.color {.uint32{0, 0, 0, 0}}
		};
		VkRenderPassBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = nullptr,
			.renderPass = pipeline.mRenderPass,
			.framebuffer = framebuffer.mFramebuffer,
			.renderArea{.offset{0, 0}, .extent{static_cast<uint32_t>(framebuffer.mSize[0]),
											   static_cast<uint32_t>(framebuffer.mSize[1])}},
			.clearValueCount = 1,
			.pClearValues = &clearValue,
		};
		vkCmdBeginRenderPass(mCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}


	void CommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(mCommandBuffer);
	}


}
