#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Image;


	class ImageView
	{
		friend class CommandBuffer;


	public:
		ImageView(const Image& image,
				  VkImageViewType viewType,
				  VkFormat format,
				  VkComponentMapping componentMapping = VkComponentMapping{
					  .r = VK_COMPONENT_SWIZZLE_IDENTITY,
					  .g = VK_COMPONENT_SWIZZLE_IDENTITY,
					  .b = VK_COMPONENT_SWIZZLE_IDENTITY,
					  .a = VK_COMPONENT_SWIZZLE_IDENTITY},
				  VkImageSubresourceRange subresourceRange = VkImageSubresourceRange{
					  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					  .baseMipLevel = 0,
					  .levelCount = 1,
					  .baseArrayLayer = 0,
					  .layerCount = 1});
		ImageView(const ImageView& rhs) = delete;
		ImageView& operator=(const ImageView& rhs) = delete;
		ImageView(ImageView&& rhs) noexcept;
		ImageView& operator=(ImageView&& rhs) noexcept;
		~ImageView();


	private:
		VkImageView mImageView;
		VkDevice mDevice;
		Core::Math::Vec3i mSize;
	};
}
