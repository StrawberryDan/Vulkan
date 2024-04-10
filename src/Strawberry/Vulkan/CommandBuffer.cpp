//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "GraphicsPipeline.hpp"
#include "Framebuffer.hpp"
#include "Fence.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "Queue.hpp"
#include "DescriptorSet.hpp"
// GLFW3
#include "GLFW/glfw3.h"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <algorithm>
#include <memory>
#include <vector>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	CommandBuffer::CommandBuffer(const CommandPool& commandPool, VkCommandBufferLevel level)
		: mCommandBuffer {}
		, mCommandPool(commandPool)
		, mExecutionFenceOrParentBuffer(ConstructExecutionFence(*commandPool.mQueue->GetDevice(), level))
	{
		VkCommandBufferAllocateInfo allocateInfo {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mCommandPool->mCommandPool,
			.level = level,
			.commandBufferCount = 1,
		};

		Core::Assert(vkAllocateCommandBuffers(*mCommandPool->GetQueue()->GetDevice(), &allocateInfo, &mCommandBuffer) == VK_SUCCESS);
	}


	CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept
		: mCommandBuffer(std::exchange(rhs.mCommandBuffer, nullptr))
		, mCommandPool(std::move(rhs.mCommandPool))
		, mState(std::exchange(rhs.mState, CommandBufferState::Invalid))
		, mOneTimeSubmission(rhs.mOneTimeSubmission)
		, mExecutionFenceOrParentBuffer(std::move(rhs.mExecutionFenceOrParentBuffer))
		, mRecordedSecondaryBuffers(std::move(rhs.mRecordedSecondaryBuffers))
	{}


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
			vkFreeCommandBuffers(*mCommandPool->GetQueue()->GetDevice(), mCommandPool->mCommandPool, 1, &mCommandBuffer);
		}
	}


	CommandBuffer::operator VkCommandBuffer() const
	{
		return mCommandBuffer;
	}


	Core::ReflexivePointer<CommandPool> CommandBuffer::GetCommandPool() const
	{
		return mCommandPool;
	}


	CommandBufferState CommandBuffer::State() const noexcept
	{
		// Check if we can move the buffer out of the Pending state.
		if (mState == CommandBufferState::Pending)
		{
			// If we own the fence we can check it normally.
			if (IsExecutionFenceSignalled())
			{
				mExecutionFenceOrParentBuffer.Ptr<Fence>()->Reset();
				MoveIntoCompletedState();
			}
		}

		return mState;
	}


	VkCommandBufferLevel CommandBuffer::Level() const noexcept
	{
		if (mExecutionFenceOrParentBuffer.IsType<Fence>())
		{
			return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		}
		else if (mExecutionFenceOrParentBuffer.IsType<Core::ReflexivePointer<CommandBuffer>>())
		{
			return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		}
		else
		{
			Core::Unreachable();
		}
	}


	void CommandBuffer::Begin(bool oneTimeSubmit)
	{
		Core::Assert(State() == CommandBufferState::Initial ||
		             State() == CommandBufferState::Invalid);

		mOneTimeSubmission = oneTimeSubmit;

		VkCommandBufferBeginInfo beginInfo {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VkCommandBufferUsageFlags(0),
			.pInheritanceInfo = nullptr,
		};

		Core::AssertEQ(vkBeginCommandBuffer(mCommandBuffer, &beginInfo), VK_SUCCESS);
		mState = CommandBufferState::Recording;
	}


	void CommandBuffer::Begin(bool oneTimeSubmit, const RenderPass& renderPass, uint32_t subpass)
	{
		Core::Assert(State() == CommandBufferState::Initial ||
		             State() == CommandBufferState::Executable);

		VkCommandBufferInheritanceInfo inheritanceInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.pNext = nullptr,
			.renderPass = renderPass,
			.subpass = subpass,
			.framebuffer = VK_NULL_HANDLE,
			.occlusionQueryEnable = VK_FALSE,
			.queryFlags = 0,
			.pipelineStatistics = 0,
		};
		VkCommandBufferBeginInfo beginInfo {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.pNext = nullptr,
				.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VkCommandBufferUsageFlags(0),
				.pInheritanceInfo = &inheritanceInfo,
		};

		Core::AssertEQ(vkBeginCommandBuffer(mCommandBuffer, &beginInfo), VK_SUCCESS);
		mState = CommandBufferState::Recording;
	}


	void CommandBuffer::Begin(bool oneTimeSubmit, const RenderPass& renderPass, uint32_t subpass,
							  const Framebuffer& framebuffer)
	{
		Core::Assert(State() == CommandBufferState::Initial);

		VkCommandBufferInheritanceInfo inheritanceInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.pNext = nullptr,
			.renderPass = renderPass,
			.subpass = subpass,
			.framebuffer = framebuffer,
			.occlusionQueryEnable = VK_FALSE,
			.queryFlags = 0,
			.pipelineStatistics = 0,
		};

		VkCommandBufferBeginInfo beginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VkCommandBufferUsageFlags(0),
			.pInheritanceInfo = &inheritanceInfo,
		};

		Core::AssertEQ(vkBeginCommandBuffer(mCommandBuffer, &beginInfo), VK_SUCCESS);
		mState = CommandBufferState::Recording;
	}


	void CommandBuffer::End()
	{
		Core::Assert(State() == CommandBufferState::Recording);
		Core::AssertEQ(vkEndCommandBuffer(mCommandBuffer), VK_SUCCESS);
		mState = CommandBufferState::Executable;
	}


	void CommandBuffer::Reset()
	{
		if (State() == CommandBufferState::Initial) return;

		Core::Assert(State() == CommandBufferState::Recording  ||
		             State() == CommandBufferState::Executable ||
					 State() == CommandBufferState::Invalid);
		Core::AssertEQ(vkResetCommandBuffer(mCommandBuffer, 0), VK_SUCCESS);
		mState = CommandBufferState::Initial;
		for (const auto& secondaryBuffer : mRecordedSecondaryBuffers) secondaryBuffer->mExecutionFenceOrParentBuffer = nullptr;
		mRecordedSecondaryBuffers.clear();
	}


	void CommandBuffer::BindPipeline(const GraphicsPipeline& pipeline)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mPipeline);
	}


	void CommandBuffer::BindVertexBuffer(uint32_t binding, Buffer& buffer, VkDeviceSize offset)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		VkBuffer handle = buffer;
		vkCmdBindVertexBuffers(mCommandBuffer, binding, 1, &handle, &offset);
	}


	void CommandBuffer::BindIndexBuffer(const Buffer& buffer, VkIndexType indexType, uint32_t offset)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdBindIndexBuffer(mCommandBuffer, buffer, offset, indexType);
	}


	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}


	void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}


	void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask,
		VkDependencyFlags dependencyFlags, const std::vector<Barrier>& barriers)
	{
		Core::Assert(State() == CommandBufferState::Recording);

		std::vector<VkMemoryBarrier> memoryBarriers;
		std::vector<VkBufferMemoryBarrier> bufferBarriers;
		std::vector<VkImageMemoryBarrier> imageBarriers;

		for (const auto& barrier : barriers)
		{
			if (barrier.IsType<ImageMemoryBarrier>())
			{
				imageBarriers.emplace_back(barrier.Value<ImageMemoryBarrier>().Value());
			}
		}

		vkCmdPipelineBarrier(mCommandBuffer,
			srcMask, dstMask,
			dependencyFlags,
			memoryBarriers.size(), memoryBarriers.data(),
			bufferBarriers.size(), bufferBarriers.data(),
			imageBarriers.size(), imageBarriers.data());
	}


	void CommandBuffer::CopyBufferToImage(const Buffer& buffer, Image& image)
	{
		Core::Assert(State() == CommandBufferState::Recording);

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
		vkCmdCopyBufferToImage(mCommandBuffer, buffer, image.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}


	void CommandBuffer::CopyImageToImage(const Image& source, VkImageLayout srcLayout, const Image& dest, VkImageLayout destLayout, VkImageAspectFlags aspect)
	{
		Core::Assert(State() == CommandBufferState::Recording);
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
		Core::Assert(State() == CommandBufferState::Recording);

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


	void CommandBuffer::ClearColorImage(Image& image, VkImageLayout layout, Core::Math::Vec4f clearColor)
	{
		Core::Assert(State() == CommandBufferState::Recording);

		VkClearColorValue vulkanClearColor {
			.float32{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}
		};

		VkImageSubresourceRange range {
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, 1,
			0, 1
		};

		vkCmdClearColorImage(mCommandBuffer, image.mImage, layout, &vulkanClearColor, 1, &range);
	}


	void CommandBuffer::BindDescriptorSet(const GraphicsPipeline& pipeline, uint32_t set, const DescriptorSet& descriptorSet)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline.mPipelineLayout, set, 1, &descriptorSet.mDescriptorSet, 0, nullptr);
	}


	void CommandBuffer::BindDescriptorSets(const GraphicsPipeline& pipeline, uint32_t firstSet, std::vector<DescriptorSet*> sets)
	{
		Core::Assert(State() == CommandBufferState::Recording);

		std::vector<VkDescriptorSet> setHandles;
		sets.reserve(sets.size());
		std::transform(sets.begin(), sets.end(), std::back_inserter(setHandles), [](DescriptorSet* set) { return set->mDescriptorSet; });
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline.mPipelineLayout, firstSet, setHandles.size(), setHandles.data(), 0, nullptr);
	}


	void CommandBuffer::BeginRenderPass(const RenderPass& renderPass, Framebuffer& framebuffer, VkSubpassContents contents)
	{
		Core::Assert(State() == CommandBufferState::Recording);

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
		vkCmdBeginRenderPass(mCommandBuffer, &beginInfo, contents);
	}


	void CommandBuffer::EndRenderPass()
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdEndRenderPass(mCommandBuffer);
	}


	void CommandBuffer::PushConstants(const GraphicsPipeline& pipeline, VkShaderStageFlags stage, const Core::IO::DynamicByteBuffer& bytes, uint32_t offset)
	{
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdPushConstants(mCommandBuffer, *pipeline.mPipelineLayout, stage, offset, bytes.Size(), bytes.Data());
	}


	void CommandBuffer::ExcecuteSecondaryBuffer(const CommandBuffer& buffer)
	{
		Core::AssertEQ(buffer.Level(), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
		Core::Assert(State() == CommandBufferState::Recording);
		vkCmdExecuteCommands(mCommandBuffer, 1, &buffer.mCommandBuffer);
	}


	Core::Variant<Fence, Core::ReflexivePointer<CommandBuffer>> CommandBuffer::ConstructExecutionFence(const Device& device, VkCommandBufferLevel level)
	{
		switch (level)
		{
			case VK_COMMAND_BUFFER_LEVEL_PRIMARY:
				return Fence(device);
			case VK_COMMAND_BUFFER_LEVEL_SECONDARY:
				return nullptr;
			default:
				Core::Unreachable();
		}
	}


	void CommandBuffer::MoveIntoPendingState() const noexcept
	{
		mState = CommandBufferState::Pending;
		for (auto secondaryBuffer : mRecordedSecondaryBuffers)
		{
			secondaryBuffer->MoveIntoPendingState();
		}
	}


	void CommandBuffer::MoveIntoCompletedState() const noexcept
	{
		mState = mOneTimeSubmission ? CommandBufferState::Invalid : CommandBufferState::Executable;

		if (mOneTimeSubmission)
		{
			mRecordedSecondaryBuffers.clear();
		}

		for (auto secondaryBuffer : mRecordedSecondaryBuffers)
		{
			secondaryBuffer->MoveIntoCompletedState();
		}
	}


	bool CommandBuffer::IsExecutionFenceSignalled() const noexcept
	{
		if (auto fence = mExecutionFenceOrParentBuffer.Ptr<Fence>())
		{
			return fence->Signaled();
		}
		else if (auto parent = mExecutionFenceOrParentBuffer.Ptr<Core::ReflexivePointer<CommandBuffer>>())
		{
			return (**parent)->IsExecutionFenceSignalled();
		}
		else
		{
			Core::Unreachable();
		}
	}
}
