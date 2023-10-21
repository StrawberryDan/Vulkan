//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ImageView.hpp"
#include "Image.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	ImageView::ImageView(const Image& image, VkImageViewType viewType, VkFormat format,
						 VkComponentMapping componentMapping, VkImageSubresourceRange subresourceRange)
		: mImageView(nullptr)
		, mDevice(image.mDevice)
		, mSize(image.mSize)
	{
		VkImageViewCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = image.mImage,
			.viewType = viewType,
			.format = format,
			.components = componentMapping,
			.subresourceRange = subresourceRange,
		};

		Core::AssertEQ(vkCreateImageView(image.mDevice, &createInfo, nullptr, &mImageView), VK_SUCCESS);
	}


	ImageView::ImageView(ImageView&& rhs) noexcept
		: mImageView(std::exchange(rhs.mImageView, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
		, mSize(std::exchange(rhs.mSize, Core::Math::Vec3i()))
	{

	}


	ImageView& ImageView::operator=(ImageView&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	ImageView::~ImageView()
	{
		if (mImageView)
		{
			vkDestroyImageView(mDevice, mImageView, nullptr);
		}
	}


	ImageView::Builder::Builder(const Image& image)
		: mImage(&image)
	{

	}


	ImageView::Builder& ImageView::Builder::WithType(VkImageViewType type)
	{
		mViewType = type;
		return *this;
	}


	ImageView::Builder& ImageView::Builder::WithFormat(VkFormat format)
	{
		mFormat = format;
		return *this;
	}


	ImageView::Builder& ImageView::Builder::WithSwizzling(VkComponentMapping mapping)
	{
		mComponentMapping = mapping;
		return *this;
	}


	ImageView::Builder& ImageView::Builder::WithSubresourceRange(VkImageSubresourceRange range)
	{
		mSubresourceRange = range;
		return *this;
	}


	ImageView ImageView::Builder::Build()
	{
		return ImageView(*mImage, *mViewType, *mFormat, mComponentMapping, mSubresourceRange);
	}
}
