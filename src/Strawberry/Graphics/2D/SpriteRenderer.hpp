#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
#include "Sprite.hpp"
#include "Strawberry/Graphics/Vulkan/Sampler.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Graphics/Vulkan/CommandPool.hpp"
#include "Strawberry/Graphics/Vulkan/Framebuffer.hpp"
#include "Strawberry/Graphics/Vulkan/Pipeline.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/Renderer.hpp"
// Standard Library
#include <set>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class SpriteRenderer
		: public Renderer
	{
	public:
		SpriteRenderer(Vulkan::Queue& queue, Core::Math::Vec2u resolution, VkFilter minFilter = VK_FILTER_NEAREST, VkFilter magFilter = VK_FILTER_NEAREST);


		void Draw(const Sprite& sprite, Transform2D transform);


	protected:
		static Vulkan::RenderPass CreateRenderPass(Vulkan::Queue& queue);
		Vulkan::Pipeline CreatePipeline();


	private:
		Vulkan::Pipeline mPipeline;
		Vulkan::CommandBuffer mCommandBuffer;


		Vulkan::DescriptorSet mVertexDescriptorSet;
		Vulkan::Buffer mCameraBuffer;
		Vulkan::Buffer mSpriteSheetBuffer;


		Vulkan::DescriptorSet mFragmentDescriptorSet;
		VkFilter mMinFilter;
		VkFilter mMagFilter;
		Vulkan::Sampler mSampler;
	};
}
