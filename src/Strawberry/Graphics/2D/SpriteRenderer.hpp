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
// Standard Library
#include <set>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class SpriteRenderer
	{
	public:
		SpriteRenderer(const Vulkan::Queue& queue, Core::Math::Vec2f viewportSize, VkFilter minFilter = VK_FILTER_NEAREST, VkFilter magFilter = VK_FILTER_NEAREST);


		void Draw(Vulkan::Framebuffer& framebuffer, const Sprite& sprite);


	protected:
		Vulkan::Pipeline CreatePipeline();


	private:
		Core::ReflexivePointer<Vulkan::Queue> mQueue;
		Core::Math::Vec2f mViewportSize;


		Vulkan::RenderPass mRenderPass;
		Vulkan::Pipeline mPipeline;
		Vulkan::CommandBuffer mCommandBuffer;


		Vulkan::Buffer mCameraBuffer;
		Vulkan::Buffer mSpriteSheetBuffer;
		VkFilter mMinFilter;
		VkFilter mMagFilter;
		Vulkan::Sampler mSampler;
	};
}
