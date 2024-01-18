//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"
#include "Framebuffer.hpp"
#include "Fence.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "Queue.hpp"
#include "DescriptorSet.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <algorithm>
#include <memory>
#include <vector>

#include "../../../cmake-build-release/_deps/glfw-src/include/GLFW/glfw3.h"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	CommandBuffer::CommandBuffer(const CommandPool& commandPool)
		: mCommandBuffer {}
		, mCommandPool(commandPool)
		, mQueueFamilyIndex(mCommandPool->GetQueue()->GetFamilyIndex())
	{
		VkCommandBufferAllocateInfo allocateInfo {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mCommandPool->mCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		Core::Assert(vkAllocateCommandBuffers(mCommandPool->GetQueue()->GetDevice()->mDevice, &allocateInfo, &mCommandBuffer) == VK_SUCCESS);
	}


	CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept
		: mCommandBuffer(std::exchange(rhs.mCommandBuffer, nullptr))
		  , mCommandPool(std::move(rhs.mCommandPool))
		  , mQueueFamilyIndex(std::exchange(rhs.mQueueFamilyIndex, 0))
	{

	}


	CommandBuffer& CommandBuffer::operator=(CommandBuffer&& rhs) noexcept
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
			vkFreeCommandBuffers(mCommandPool->GetQueue()->GetDevice()->mDevice, mCommandPool->mCommandPool, 1, &mCommandBuffer);
		}
	}


	Core::ReflexivePointer<CommandPool> CommandBuffer::GetCommandPool() const
	{
		return mCommandPool;
	}


	void CommandBuffer::Begin(bool oneTimeSubmit)
	{
		VkCommandBufferBeginInfo beginInfo {
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


	void CommandBuffer::BindVertexBuffer(uint32_t binding, Buffer& buffer, VkDeviceSize offset)
	{
		vkCmdBindVertexBuffers(mCommandBuffer, binding, 1, &buffer.mBuffer, &offset);
	}


	void CommandBuffer::BindIndexBuffer(const Buffer& buffer, VkIndexType indexType, uint32_t offset)
	{
		vkCmdBindIndexBuffer(mCommandBuffer, buffer.mBuffer, offset, indexType);
	}


	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset)
	{
		vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}


	void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset)
	{
		vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}


	void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
										const std::vector<VkMemoryBarrier>& memoryBarriers,
										const std::vector<VkBufferMemoryBarrier>& bufferBarriers,
										const std::vector<VkImageMemoryBarrier>& imageBarriers)
	{
		vkCmdPipelineBarrier(mCommandBuffer,
			srcMask, dstMask,
			dependencyFlags,
			memoryBarriers.size(), memoryBarriers.data(),
			bufferBarriers.size(), bufferBarriers.data(),
			imageBarriers.size(), imageBarriers.data());
	}


	void CommandBuffer::CopyBufferToImage(const Buffer& buffer, Image& image)
	{
		// Setup copy
		VkImageSubresourceLayers subresource {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};
		VkBufferImageCopy region {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = subresource,
			.imageOffset{.x = 0, .y = 0, .z = 0},
			.imageExtent{.width = image.mSize[0], .height = image.mSize[1], .depth = 1}
		};
		vkCmdCopyBufferToImage(mCommandBuffer, buffer.mBuffer, image.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}


	void CommandBuffer::CopyImageToImage(const Image& source, VkImageLayout srcLayout, const Image& dest, VkImageLayout destLayout, VkImageAspectFlags aspect)
	{
		Core::AssertEQ(source.GetSize(), dest.GetSize());


		VkImageCopy region {
			.srcSubresource = VkImageSubresourceLayers {
				.aspectMask = aspect,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.srcOffset = {0, 0, 0},
			.dstSubresource = VkImageSubresourceLayers {
				.aspectMask = aspect,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.dstOffset = {0,0, 0},
			.extent = VkExtent3D{ source.GetSize()[0], source.GetSize()[1], source.GetSize()[2] }
		};
		vkCmdCopyImage(mCommandBuffer, source.mImage, srcLayout, dest.mImage, destLayout, 1, &region);
	}


	void CommandBuffer::BlitImage(const Image& source, VkImageLayout srcLayout, const Image& dest, VkImageLayout destLayout, VkImageAspectFlags aspect, VkFilter filter)
	{
		VkImageBlit region {
			.srcSubresource = VkImageSubresourceLayers {
				.aspectMask = aspect,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.srcOffsets = {VkOffset3D{0, 0, 0}, VkOffset3D{(int) source.GetSize()[0], (int) source.GetSize()[1], (int) source.GetSize()[2]}},
			.dstSubresource = VkImageSubresourceLayers {
				.aspectMask = aspect,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.dstOffsets = {VkOffset3D{0, 0, 0}, VkOffset3D{(int) dest.GetSize()[0], (int) dest.GetSize()[1], (int) dest.GetSize()[2]}},
		};

		vkCmdBlitImage(mCommandBuffer, source.mImage, srcLayout, dest.mImage, destLayout, 1, &region, filter);
	}


	void CommandBuffer::ClearColorImage(Image& image, Core::Math::Vec4f clearColor)
	{
		VkClearColorValue vulkanClearColor {
			.float32{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}
		};

		VkImageSubresourceRange range {
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, 1,
			0, 1
		};

		vkCmdClearColorImage(mCommandBuffer, image.mImage, VK_IMAGE_LAYOUT_GENERAL, &vulkanClearColor, 1, &range);
	}


	void CommandBuffer::BindDescriptorSet(const Pipeline& pipeline, uint32_t set, const DescriptorSet& descriptorSet)
	{
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mPipelineLayout, set, 1, &descriptorSet.mDescriptorSet, 0, nullptr);
	}


	void CommandBuffer::BindDescriptorSets(const Pipeline& pipeline, uint32_t firstSet, std::vector<DescriptorSet*> sets)
	{
		std::vector<VkDescriptorSet> setHandles;
		sets.reserve(sets.size());
		std::transform(sets.begin(), sets.end(), std::back_inserter(setHandles), [](DescriptorSet* set) { return set->mDescriptorSet; });
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mPipelineLayout, firstSet, setHandles.size(), setHandles.data(), 0, nullptr);
	}


	void CommandBuffer::BeginRenderPass(const RenderPass& renderPass, Framebuffer& framebuffer)
	{
		VkRenderPassBeginInfo beginInfo {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = nullptr,
			.renderPass = renderPass.mRenderPass,
			.framebuffer = framebuffer.mFramebuffer,
			.renderArea{.offset{0, 0}, .extent{static_cast<uint32_t>(framebuffer.mSize[0]),
											   static_cast<uint32_t>(framebuffer.mSize[1])}},
			.clearValueCount = static_cast<uint32_t>(renderPass.mClearColors.size()),
			.pClearValues = renderPass.mClearColors.data(),
		};
		vkCmdBeginRenderPass(mCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}


	void CommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(mCommandBuffer);
	}


	void CommandBuffer::PushConstants(const Pipeline& pipeline, VkShaderStageFlags stage, const Core::IO::DynamicByteBuffer& bytes, uint32_t offset)
	{
		vkCmdPushConstants(mCommandBuffer, pipeline.mPipelineLayout, stage, offset, bytes.Size(), bytes.Data());
	}
}
