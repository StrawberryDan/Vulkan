#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ImageView.hpp"
#include "Swapchain.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Standard Library
#include <vector>



//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class CommandPool;
	class Buffer;
	class Pipeline;
	class Framebuffer;
	class RenderPass;
	class DescriptorSet;


	class CommandBuffer
	{
		friend class Queue;


	public:
		explicit CommandBuffer(const Queue& queue);
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


		void ImageMemoryBarrier(Image& image, VkImageAspectFlagBits aspect, VkImageLayout targetLayout);
		void ImageMemoryBarrier(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout oldLayout,
		                        VkImageLayout targetLayout);


		void CopyImageToSwapchain(Image& image, Swapchain& swapchain);
		void CopyBufferToImage(const Buffer& buffer, Image& image);


		void BindDescriptorSet(const Pipeline& pipeline, uint32_t set, const DescriptorSet& descriptorSet);


	private:
		VkCommandBuffer mCommandBuffer;
		Core::ReflexivePointer<CommandPool> mCommandPool;
		uint32_t mQueueFamilyIndex;
	};
}
