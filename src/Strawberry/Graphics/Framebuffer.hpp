#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Image.hpp"
#include "ImageView.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;
	class Pipeline;
	class ImageView;


	class Framebuffer
	{
		friend class CommandBuffer;


	public:
		Framebuffer(const Pipeline& view, Core::Math::Vec2i size, uint32_t colorAttachmentCount = 1);
		Framebuffer(const Framebuffer& rhs) = delete;
		Framebuffer& operator=(const Framebuffer& rhs) = delete;
		Framebuffer(Framebuffer&& rhs) noexcept;
		Framebuffer& operator=(Framebuffer&& rhs) noexcept;
		~Framebuffer();


		uint32_t GetColorAttachmentCount() const;
		const Image& GetColorAttachment(uint32_t index);
		const Image& GetDepthAttachment();
		const Image& GetStencilAttachment();


	private:
		Image CreateDepthImage(const Pipeline& pipeline);
		ImageView CreateDepthImageView();
		Image CreateStencilImage(const Pipeline& pipeline);
		ImageView CreateStencilImageView();


	private:
		VkFramebuffer mFramebuffer;
		const Device* mDevice;
		Core::Math::Vec2i mSize;


		std::vector<Image> mColorAttachments;
		std::vector<ImageView> mColorAttachmentViews;

		Image mDepthAttachment;
		ImageView mDepthAttachmentView;
		Image mStencilAttachment;
		ImageView mStencilAttachmentView;
	};
}
