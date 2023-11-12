#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Vulkan/Framebuffer.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Math/Vector.hpp"

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Renderer
	{
	public:
		void SetFramebuffer(Vulkan::Framebuffer&& framebuffer);
		Vulkan::Framebuffer TakeFramebuffer();


	protected:
		Renderer(Vulkan::Queue& queue, Core::Math::Vec2u resolution);
		Renderer(Vulkan::Queue& queue, Vulkan::RenderPass& renderPass, Core::Math::Vec2u resolution);


		void SetRenderPass(Vulkan::RenderPass& renderPass);


		Vulkan::Framebuffer& GetFramebuffer();


		Core::Math::Vec2u GetResolution() const;
		Core::ReflexivePointer<Vulkan::Queue> GetQueue();
		Core::ReflexivePointer<Vulkan::RenderPass> GetRenderPass();


	private:
		Core::Math::Vec2u mResolution;
		Core::ReflexivePointer<Vulkan::Queue> mQueue;
		Core::ReflexivePointer<Vulkan::RenderPass> mRenderPass;
		Core::Optional<Vulkan::Framebuffer> mFramebuffer;
	};
}