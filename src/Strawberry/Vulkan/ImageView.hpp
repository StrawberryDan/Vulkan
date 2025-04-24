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
namespace Strawberry::Vulkan
{
	class Image;


	class ImageView
	{
	public:
		class Builder;

	public:
		ImageView(const ImageView& rhs)            = delete;
		ImageView& operator=(const ImageView& rhs) = delete;
		ImageView(ImageView&& rhs) noexcept;
		ImageView& operator=(ImageView&& rhs) noexcept;
		~ImageView();


		operator VkImageView() const;

	private:
		ImageView(const Image&            image,
		          VkImageViewType         viewType,
		          VkFormat                format,
		          VkComponentMapping      componentMapping,
		          VkImageSubresourceRange subresourceRange);

	private:
		VkImageView mImageView;
		VkDevice    mDevice;
	};


	class ImageView::Builder
	{
	public:
		Builder(const Image& image, VkImageAspectFlags aspectFlags);


		Builder& WithType(VkImageViewType type);
		Builder& WithFormat(VkFormat format);


		Builder& WithSubresourceRange(VkImageSubresourceRange range);


		Builder& WithSwizzling(VkComponentMapping mapping);


		ImageView Build();

	private:
		const Image*                    mImage;
		Core::Optional<VkImageViewType> mViewType;
		Core::Optional<VkFormat>        mFormat;

		VkComponentMapping mComponentMapping{
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		};

		VkImageSubresourceRange mSubresourceRange{
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};
	};
}
