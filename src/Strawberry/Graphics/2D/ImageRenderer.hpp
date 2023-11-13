#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Renderer.hpp"
#include "Strawberry/Graphics/Vulkan/Pipeline.hpp"
#include "Strawberry/Graphics/Vulkan/Sampler.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class ImageRenderer
		: public Renderer
	{
	public:
		ImageRenderer(Vulkan::Queue& queue, Core::Math::Vec2u resolution);


		void Draw(Vulkan::Image& image, Core::Math::Vec2f position);
		void Draw(Vulkan::Image& image, Core::Math::Vec2f position, Core::Math::Vec2f size);


	protected:
		static Vulkan::RenderPass CreateRenderPass(Vulkan::Device& device);
		Vulkan::Pipeline CreatePipeline();


	private:
		Core::ReflexivePointer<Vulkan::Queue> mQueue;
		Vulkan::Pipeline mPipeline;
		Vulkan::DescriptorSet mDescriptorSet;
		Vulkan::Buffer mVertexUniformBuffer;
		static const VkFilter mMinFilter = VK_FILTER_NEAREST;
		static const VkFilter mMagFilter = VK_FILTER_NEAREST;
		Vulkan::Sampler mTextureSampler;
	};
}