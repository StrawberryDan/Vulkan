#pragma once


#include "Strawberry/Vulkan/Resource/Image.hpp"


namespace Strawberry::Vulkan
{
	class CommandBlitImage
	{
	public:
		friend class CommandBuffer;

		CommandBlitImage& WithSource(const Image& src, VkImageLayout srcLayout)
		{
			this->mSrc       = &src;
			this->mSrcLayout = srcLayout;
			return *this;
		}

		CommandBlitImage& WithDest(Image& dst, VkImageLayout dstLayout)
		{
			this->mDst = &dst;
			this->mDstLayout = dstLayout;
			WithDestExtent(dst.GetSize());
			return *this;
		}

		CommandBlitImage& WithAspect(VkImageAspectFlags aspect)
		{
			this->mAspect = aspect;
			return *this;
		}

		CommandBlitImage& WithFilter(VkFilter filter)
		{
			this->mFilter = filter;
			return *this;
		}

		CommandBlitImage& WithDestOffset(const Core::Math::Vec3u& offset)
		{
			this->mDstOffset = offset;
			return *this;
		}

		CommandBlitImage& WithDestExtent(const Core::Math::Vec3u& extent)
		{
			this->mDstExtent = extent;
			return *this;
		}

	private:
		const Image*                      mSrc;
		VkImageLayout                     mSrcLayout;
		Image*                            mDst;
		VkImageLayout                     mDstLayout;
		VkImageAspectFlags                mAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		VkFilter                          mFilter = VK_FILTER_NEAREST;
		Core::Math::Vec3u                 mDstOffset{0, 0, 0};
		Core::Optional<Core::Math::Vec3u> mDstExtent;
	};


	class CommandCopyBufferToImage
	{
	public:
		friend class CommandBuffer;


		CommandCopyBufferToImage& WithAspect(VkImageAspectFlags aspect)
		{
			this->mAspect = aspect;
			return *this;
		}

		CommandCopyBufferToImage& WithSrcBuffer(const Buffer& buffer)
		{
			mSrcBuffer = &buffer;
			return *this;
		}

		CommandCopyBufferToImage& WithDstImage(Image& image)
		{
			mDstImage = &image;
			return *this;
		}

		CommandCopyBufferToImage& WithDstArrayLayer(uint32_t layer)
		{
			mDstArrayLayer = layer;
			return *this;
		}

		CommandCopyBufferToImage& WithDstOffset(const Core::Math::Vec3u& offset)
		{
			mDstOffset = offset;
			return *this;
		}

		CommandCopyBufferToImage& WithDstExtent(const Core::Math::Vec3u& extent)
		{
			mDstExtent = extent;
			return *this;
		}


	private:
		VkImageAspectFlags                mAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		const Buffer*                     mSrcBuffer;
		Image*                            mDstImage;
		uint32_t                          mDstArrayLayer = 0;
		Core::Math::Vec3u                 mDstOffset;
		Core::Optional<Core::Math::Vec3u> mDstExtent;
	};
}
