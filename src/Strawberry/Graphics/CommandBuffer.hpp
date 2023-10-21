#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ImageView.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class CommandPool;
	class Buffer;
	class Pipeline;


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


		void BeginRenderPass(Strawberry::Graphics::ImageView& colorAttachment);
		void EndRenderPass();


		void BindVertexBuffer(uint32_t binding, Buffer& buffer);
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t vertexOffset = 0, uint32_t instanceOffset = 0);


	private:
		VkCommandBuffer mCommandBuffer;
		VkCommandPool mCommandPool;
		VkDevice mDevice;
	};
}
