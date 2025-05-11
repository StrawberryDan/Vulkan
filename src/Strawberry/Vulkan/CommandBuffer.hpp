#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Fence.hpp"
#include "ImageView.hpp"
#include "ImageMemoryBarrier.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Variant.hpp"
// Standard Library
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class CommandPool;
	class Buffer;
	class ComputePipeline;
	class GraphicsPipeline;
	class Framebuffer;
	class RenderPass;
	class DescriptorSet;
	class Swapchain;


	using Barrier = Core::Variant<ImageMemoryBarrier>;


	enum class CommandBufferState
	{
		Initial,
		Recording,
		Executable,
		Pending,
		Invalid,
	};


	class CommandBuffer
			: public Core::EnableReflexivePointer
	{
		friend class Queue;

	public:
		explicit CommandBuffer(CommandPool& commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);


		CommandBuffer(const CommandBuffer& rhs)            = delete;
		CommandBuffer& operator=(const CommandBuffer& rhs) = delete;
		CommandBuffer(CommandBuffer&& rhs) noexcept;
		CommandBuffer& operator=(CommandBuffer&& rhs) noexcept;
		~CommandBuffer();


		operator VkCommandBuffer() const;


		Core::ReflexivePointer<CommandPool> GetCommandPool() const;


		CommandBufferState State() const noexcept;


		VkCommandBufferLevel Level() const noexcept;


		void Begin(bool oneTimeSubmit = false);
		void Begin(bool oneTimeSubmit, const RenderPass& renderPass, uint32_t subpass);
		void Begin(bool oneTimeSubmit, const RenderPass& renderPass, uint32_t subpass, const Framebuffer& framebuffer);
		void End();
		void Reset();


		void BindPipeline(const GraphicsPipeline& pipeline);
		void BindPipeline(const ComputePipeline& pipeline);


		void Dispatch(size_t x);
		void Dispatch(size_t x, size_t y);
		void Dispatch(size_t x, size_t y, size_t z);

		void Dispatch(Core::Math::Vec2u xy);
		void Dispatch(Core::Math::Vec3u xyz);


		void BeginRenderPass(const RenderPass& renderPass, Framebuffer& framebuffer, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
		void EndRenderPass();


		void ExcecuteSecondaryBuffer(const CommandBuffer& buffer);


		void PushConstants(const GraphicsPipeline& pipeline, VkShaderStageFlags stage, const Core::IO::DynamicByteBuffer& bytes, uint32_t offset);


		void BindVertexBuffer(uint32_t binding, Buffer& buffer, VkDeviceSize offset = 0);
		void BindIndexBuffer(const Buffer& buffer, VkIndexType indexType, uint32_t offset = 0);
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t vertexOffset = 0, uint32_t instanceOffset = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t firstInstance = 0, int32_t vertexOffset = 0);


		void PipelineBarrier(VkPipelineStageFlags        srcMask,
		                     VkPipelineStageFlags        dstMask,
		                     VkDependencyFlags           dependencyFlags,
		                     const std::vector<Barrier>& barriers);


		void CopyBufferToImage(const Buffer& buffer, Image& image);
		void CopyImageToImage(const Image& source, VkImageLayout srcLayout, const Image& dest, VkImageLayout destLayout, VkImageAspectFlags aspect);
		void BlitImage(const Image&       source,
		               VkImageLayout      srcLayout,
		               const Image&       dest,
		               VkImageLayout      destLayout,
		               VkImageAspectFlags aspect,
		               VkFilter           filter);
		void ClearColorImage(Image& image, VkImageLayout layout, Core::Math::Vec4f clearColor = {0.0f, 0.0f, 0.0f, 1.0f});


		void BindDescriptorSet(const GraphicsPipeline& pipeline, uint32_t set, const DescriptorSet& descriptorSet);
		void BindDescriptorSets(const GraphicsPipeline& pipeline, uint32_t firstSet, std::vector<DescriptorSet*> sets);

		void BindDescriptorSet(const ComputePipeline& pipeline, uint32_t set, const DescriptorSet& descriptorSet);
		void BindDescriptorSets(const ComputePipeline& pipeline, uint32_t firstSet, std::vector<DescriptorSet*> sets);


	private:
		static Core::Variant<Fence, Core::ReflexivePointer<CommandBuffer>> ConstructExecutionFence(const Device& device, VkCommandBufferLevel level);
		void                                                               MoveIntoPendingState() const noexcept;
		void                                                               MoveIntoCompletedState() const noexcept;
		bool                                                               IsExecutionFenceSignalled() const noexcept;

	private:
		VkCommandBuffer                                                     mCommandBuffer;
		Core::ReflexivePointer<CommandPool>                                 mCommandPool;
		mutable CommandBufferState                                          mState = CommandBufferState::Initial;
		bool                                                                mOneTimeSubmission;
		mutable Core::Variant<Fence, Core::ReflexivePointer<CommandBuffer>> mExecutionFenceOrParentBuffer;

		mutable std::vector<Core::ReflexivePointer<CommandBuffer>> mRecordedSecondaryBuffers;
	};
}
