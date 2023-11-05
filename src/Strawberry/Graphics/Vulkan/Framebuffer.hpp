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
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Standard Library
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;
	class RenderPass;
	class ImageView;


	class Framebuffer
		: public Core::EnableReflexivePointer<Framebuffer>
	{
		friend class CommandBuffer;


	public:
		Framebuffer(const RenderPass& mRenderPass, Core::Math::Vec2u size);
		Framebuffer(const Framebuffer& rhs) = delete;
		Framebuffer& operator=(const Framebuffer& rhs) = delete;
		Framebuffer(Framebuffer&& rhs) noexcept;
		Framebuffer& operator=(Framebuffer&& rhs) noexcept;
		~Framebuffer();


		[[nodiscard]] Core::ReflexivePointer<RenderPass> GetRenderPass() const;


		[[nodiscard]] Core::Math::Vec2u GetSize() const;


		[[nodiscard]] uint32_t GetColorAttachmentCount() const;
		Image& GetColorAttachment(uint32_t index);
		Image& GetDepthAttachment();
		Image& GetStencilAttachment();


	private:
		Image CreateDepthImage();
		ImageView CreateDepthImageView();
		Image CreateStencilImage();
		ImageView CreateStencilImageView();


	private:
		VkFramebuffer mFramebuffer;
		Core::ReflexivePointer<RenderPass> mRenderPass;
		Core::Math::Vec2u mSize;


		std::vector<Image> mColorAttachments;
		std::vector<ImageView> mColorAttachmentViews;


		Image mDepthAttachment;
		ImageView mDepthAttachmentView;
		Image mStencilAttachment;
		ImageView mStencilAttachmentView;
	};
}
