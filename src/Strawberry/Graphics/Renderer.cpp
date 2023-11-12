//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Renderer.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Renderer::Renderer(Vulkan::Queue& queue, Core::Math::Vec2u resolution)
			: mResolution(resolution)
			, mQueue(queue)
	{}


	Renderer::Renderer(Vulkan::Queue& queue, Vulkan::RenderPass renderPass, Core::Math::Vec2u resolution)
		: mResolution(resolution)
		, mQueue(queue)
		, mRenderPass(std::move(renderPass))
	{}


	bool Renderer::FramebufferAvailable() const
	{
		return mFramebuffer.HasValue();
	}


	void Renderer::SetFramebuffer(Vulkan::Framebuffer&& framebuffer)
	{
		mFramebuffer = std::forward<Vulkan::Framebuffer&&>(framebuffer);
	}


	Vulkan::Framebuffer Renderer::TakeFramebuffer()
	{
		if (!mFramebuffer)
		{
			mFramebuffer = Vulkan::Framebuffer(*mRenderPass, mResolution);
			for (int i = 0; i < mFramebuffer->GetColorAttachmentCount(); i++)
				mFramebuffer->GetColorAttachment(i).ClearColor(*mQueue);
		}

		return mFramebuffer.Unwrap();
	}


	Vulkan::Framebuffer& Renderer::GetFramebuffer()
	{
		if (!mFramebuffer)
		{
			mFramebuffer = Vulkan::Framebuffer(*mRenderPass, mResolution);
			for (int i = 0; i < mFramebuffer->GetColorAttachmentCount(); i++)
				mFramebuffer->GetColorAttachment(i).ClearColor(*mQueue);
		}

		return mFramebuffer.Value();
	}


	Core::Math::Vec2u Renderer::GetResolution() const
	{
		return mResolution;
	}


	Core::ReflexivePointer<Vulkan::Queue> Renderer::GetQueue()
	{
		return mQueue;
	}


	Core::ReflexivePointer<Vulkan::RenderPass> Renderer::GetRenderPass()
	{
		return mRenderPass->GetReflexivePointer();
	}
}