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


	class CommandBuffer
	{
		friend class Queue;


	public:
		explicit CommandBuffer(const CommandPool& commandPool);
		CommandBuffer(const CommandBuffer& rhs) = delete;
		CommandBuffer& operator=(const CommandBuffer& rhs) = delete;
		CommandBuffer(CommandBuffer&& rhs) noexcept ;
		CommandBuffer& operator=(CommandBuffer&& rhs);
		~CommandBuffer();


		void Begin(bool oneTimeSubmit);
		void End();
		void Reset();


		void BindPipeline(const Pipeline& pipeline);


		void BeginRenderPass(const RenderPass& pipeline, Framebuffer& framebuffer);
		void EndRenderPass();


		void PushConstants(const Pipeline& pipeline, VkShaderStageFlags stage, const Core::IO::DynamicByteBuffer& bytes, uint32_t offset);


		void BindVertexBuffer(uint32_t binding, Buffer& buffer);
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t vertexOffset = 0, uint32_t instanceOffset = 0);


		void ImageMemoryBarrier(const Image& image, VkImageAspectFlagBits aspect, VkImageLayout targetLayout);
		void ImageMemoryBarrier(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout targetLayout);


		void CopyImageToSwapchain(const Image& image, Swapchain& swapchain);
		void CopyBufferToImage(const Buffer& buffer, Image& image, VkFormat format);


		void BindDescriptorSet(const Pipeline& pipeline, uint32_t set);


	private:
		VkCommandBuffer mCommandBuffer;
		VkCommandPool mCommandPool;
		VkDevice mDevice;
		uint32_t mQueueFamilyIndex;
	};
}
