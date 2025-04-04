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
namespace Strawberry::Vulkan
{
	class Device;
	class RenderPass;
	class ImageView;


	class Framebuffer
	: public Core::EnableReflexivePointer
	{
		friend class CommandBuffer;

	public:
		Framebuffer(const RenderPass& mRenderPass, Allocator& allocator, Core::Math::Vec2u size);
		Framebuffer(const Framebuffer& rhs)            = delete;
		Framebuffer& operator=(const Framebuffer& rhs) = delete;
		Framebuffer(Framebuffer&& rhs) noexcept;
		Framebuffer& operator=(Framebuffer&& rhs) noexcept;
		~Framebuffer();


		operator VkFramebuffer() const noexcept;


		[[nodiscard]] Core::ReflexivePointer<RenderPass> GetRenderPass() const;
		[[nodiscard]] Core::ReflexivePointer<Device> GetDevice() const;
		[[nodiscard]] Core::ReflexivePointer<PhysicalDevice> GetPhysicalDevice() const;


		[[nodiscard]] Core::Math::Vec2u GetSize() const;


		[[nodiscard]] uint32_t GetColorAttachmentCount() const;
		Image&                 GetColorAttachment(uint32_t index);
		Image&                 GetDepthAttachment();
		Image&                 GetStencilAttachment();

	private:
		Image     CreateDepthImage(const Device& device, Allocator& allocator);
		ImageView CreateDepthImageView();
		Image     CreateStencilImage(const Device& device, Allocator& allocator);
		ImageView CreateStencilImageView();


		VkFramebuffer                      mFramebuffer;
		Core::ReflexivePointer<RenderPass> mRenderPass;
		Core::Math::Vec2u                  mSize;


		std::vector<Image>     mColorAttachments;
		std::vector<ImageView> mColorAttachmentViews;


		Image     mDepthAttachment;
		ImageView mDepthAttachmentView;
		Image     mStencilAttachment;
		ImageView mStencilAttachmentView;
	};
}
