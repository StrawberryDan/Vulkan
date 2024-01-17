#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ImageView.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Standard Library
#include <vector>



//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class CommandPool;
	class Buffer;
	class Pipeline;
	class Framebuffer;
	class RenderPass;
	class DescriptorSet;
	class Swapchain;


	class CommandBuffer
	{
		friend class Queue;


	public:
		explicit CommandBuffer(const CommandPool& commandPool);
		CommandBuffer(const CommandBuffer& rhs) = delete;
		CommandBuffer& operator=(const CommandBuffer& rhs) = delete;
		CommandBuffer(CommandBuffer&& rhs) noexcept ;
		CommandBuffer& operator=(CommandBuffer&& rhs) noexcept ;
		~CommandBuffer();


		Core::ReflexivePointer<CommandPool> GetCommandPool() const;


		void Begin(bool oneTimeSubmit);
		void End();
		void Reset();


		void BindPipeline(const Pipeline& pipeline);


		void BeginRenderPass(const RenderPass& renderPass, Framebuffer& framebuffer);
		void EndRenderPass();


		void PushConstants(const Pipeline& pipeline, VkShaderStageFlags stage, const Core::IO::DynamicByteBuffer& bytes, uint32_t offset);


		void BindVertexBuffer(uint32_t binding, Buffer& buffer);
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t vertexOffset = 0, uint32_t instanceOffset = 0);


		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 const std::vector<VkMemoryBarrier>&       memoryBarriers,
							 const std::vector<VkBufferMemoryBarrier>& bufferBarriers,
							 const std::vector<VkImageMemoryBarrier>&  imageBarriers);

		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 const std::vector<VkMemoryBarrier>& memoryBarriers)
		{
			PipelineBarrier(srcMask, dstMask, dependencyFlags, memoryBarriers, {}, {});
		}

		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 VkMemoryBarrier memoryBarrier)
		{
			PipelineBarrier(srcMask, dstMask, dependencyFlags, {memoryBarrier}, {}, {});
		}

		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 const std::vector<VkBufferMemoryBarrier>& bufferBarriers)
		{
			PipelineBarrier(srcMask, dstMask, dependencyFlags, {}, bufferBarriers, {});
		}

		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 VkBufferMemoryBarrier bufferBarrier)
		{
			PipelineBarrier(srcMask, dstMask, dependencyFlags, {}, {bufferBarrier}, {});
		}

		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 const std::vector<VkImageMemoryBarrier>& imageBarriers)
		{
			PipelineBarrier(srcMask, dstMask, dependencyFlags, {}, {}, imageBarriers);
		}

		void PipelineBarrier(VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, VkDependencyFlags dependencyFlags,
							 VkImageMemoryBarrier imageBarrier)
		{
			PipelineBarrier(srcMask, dstMask, dependencyFlags, {}, {}, {imageBarrier});
		}


		void CopyImageToImage(const Image& source, VkImageLayout srcLayout, const Image& dest, VkImageLayout destLayout, VkImageAspectFlags aspect);
		void BlitImage(const Image& source, VkImageLayout srcLayout, const Image& dest, VkImageLayout destLayout, VkImageAspectFlags aspect, VkFilter filter);
		void CopyBufferToImage(const Buffer& buffer, Image& image);
		void ClearColorImage(Image& image, Core::Math::Vec4f clearColor = {0.0f, 0.0f, 0.0f, 1.0f});


		void BindDescriptorSet(const Pipeline& pipeline, uint32_t set, const DescriptorSet& descriptorSet);


	private:
		VkCommandBuffer mCommandBuffer;
		Core::ReflexivePointer<CommandPool> mCommandPool;
		uint32_t mQueueFamilyIndex;
	};
}
