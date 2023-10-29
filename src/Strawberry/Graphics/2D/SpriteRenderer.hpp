#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
#include "Sprite.hpp"
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
		SpriteRenderer(const Vulkan::Queue& queue, Core::Math::Vec2f viewportSize);


		void BeginRenderPass(Vulkan::Framebuffer& renderPass);
		void EndRenderPass();
		void Draw(const Sprite& sprite);


	protected:
		Vulkan::Pipeline CreatePipeline();


	private:
		Core::ReflexivePointer<Vulkan::Queue> mQueue;
		Core::Math::Vec2f mViewportSize;
		Vulkan::Buffer mCameraBuffer;

		Vulkan::RenderPass mRenderPass;
		Vulkan::Pipeline mPipeline;
		Vulkan::CommandPool mCommandPool;

		Core::Optional<Vulkan::CommandBuffer> mRenderPassBuffer;
		std::set<Core::ReflexivePointer<Sprite>> mSprites;

	};
}
