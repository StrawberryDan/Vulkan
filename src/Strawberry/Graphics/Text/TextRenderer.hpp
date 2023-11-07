#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "FontFace.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
#include "Strawberry/Graphics/Vulkan/Pipeline.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/Vulkan/Framebuffer.hpp"
#include "Strawberry/Graphics/Vulkan/Sampler.hpp"
// Strawberry Core
#include <Strawberry/Core/Types/ReflexivePointer.hpp>
// Standard Library
#include <string>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class TextRenderer
	{
	public:
		TextRenderer(const Vulkan::Queue& queue, Core::Math::Vec2u renderSize);



		void Draw(const FontFace& fontface, Core::Math::Vec2i position, const std::string& string);
		void Draw(const FontFace& fontface, Core::Math::Vec2i position, const std::u32string& string);


		void SetFramebuffer(Vulkan::Framebuffer framebuffer);
		Vulkan::Framebuffer GetFramebuffer();


	protected:
		Vulkan::RenderPass CreateRenderPass(const Vulkan::Device& device);
		Vulkan::Pipeline CreatePipeline(const Vulkan::RenderPass& renderPass, Core::Math::Vec2u renderSize);



	private:
		Core::ReflexivePointer<Vulkan::Queue> mQueue;
		Core::Math::Vec2u mRenderSize;
		Vulkan::RenderPass mRenderPass;
		Vulkan::Pipeline mPipeline;
		Core::Optional<Vulkan::Framebuffer> mFrameBuffer;
		Vulkan::DescriptorSet mDescriptorSet;
		Vulkan::Buffer mPassConstantsBuffer;
		Vulkan::Buffer mDrawConstantsBuffer;
		Vulkan::Sampler mSampler;
	};
}
