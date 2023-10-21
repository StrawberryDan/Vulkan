#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
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
		friend class Builder;


		friend class CommandBuffer;


		friend class Framebuffer;


	public:
		class Builder;


	public:
		ImageView(const ImageView& rhs) = delete;
		ImageView& operator=(const ImageView& rhs) = delete;
		ImageView(ImageView&& rhs) noexcept;
		ImageView& operator=(ImageView&& rhs) noexcept;
		~ImageView();


	private:
		ImageView(const Image& image,
				  VkImageViewType viewType,
				  VkFormat format,
				  VkComponentMapping componentMapping,
				  VkImageSubresourceRange subresourceRange);


	private:
		VkImageView mImageView;
		VkDevice mDevice;
		Core::Math::Vec3i mSize;
	};


	class ImageView::Builder
	{
	public:
		Builder(const Image& image);


		Builder& WithFormat(VkFormat format);


		Builder& WithType(VkImageViewType type);


//		Builder& WithSize(uint32_t size);
//		Builder& WithSize(Core::Math::Vec2i size);
//		Builder& WithSize(Core::Math::Vec3i size);


		Builder& WithSwizzling(VkComponentMapping mapping);


		Builder& WithSubresourceRange(VkImageSubresourceRange range);


		ImageView Build();

	private:
		const Image* mImage;

		Core::Optional<VkImageViewType> mViewType;
		Core::Optional<VkFormat> mFormat;
		Core::Optional<Core::Math::Vec3i> mSize;

		VkComponentMapping mComponentMapping{
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		};

		VkImageSubresourceRange mSubresourceRange{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};
	};
}
