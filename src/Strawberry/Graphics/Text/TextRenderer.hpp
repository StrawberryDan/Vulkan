#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "FontFace.hpp"
#include "Strawberry/Graphics/Renderer.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
#include "Strawberry/Graphics/Vulkan/Pipeline.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/Vulkan/Framebuffer.hpp"
#include "Strawberry/Graphics/Vulkan/Sampler.hpp"
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
		: public Renderer
	{
	public:
		TextRenderer(Vulkan::Queue& queue, Vulkan::RenderPass& renderPass, Core::Math::Vec2u resolution);



		void Draw(const FontFace& fontface, const std::string& string, Core::Math::Vec2i position, Core::Math::Vec4f color);
		void Draw(const FontFace& fontface, const std::u32string& string, Core::Math::Vec2i position, Core::Math::Vec4f color);


	protected:
		Vulkan::Pipeline CreatePipeline(const Vulkan::RenderPass& renderPass, Core::Math::Vec2u renderSize);



	private:
		Vulkan::Pipeline mPipeline;
		Vulkan::DescriptorSet mDescriptorSet;
		Vulkan::Buffer mPassConstantsBuffer;
		Vulkan::Buffer mDrawConstantsBuffer;
		Vulkan::Sampler mSampler;
		Vulkan::Buffer mFragDrawConstantsBuffer;
	};
}
