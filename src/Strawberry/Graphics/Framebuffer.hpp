#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"


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
		Framebuffer(const Pipeline& pipeline, const ImageView& colorAttachment);
		Framebuffer(const Framebuffer& rhs) = delete;
		Framebuffer& operator=(const Framebuffer& rhs) = delete;
		Framebuffer(Framebuffer&& rhs) noexcept;
		Framebuffer& operator=(Framebuffer&& rhs) noexcept;
		~Framebuffer();


	private:
		VkFramebuffer mFramebuffer;
		VkDevice mDevice;
		Core::Math::Vec2i mSize;
	};
}
