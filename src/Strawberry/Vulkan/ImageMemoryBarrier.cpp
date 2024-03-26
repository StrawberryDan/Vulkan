//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ImageMemoryBarrier.hpp"
#include "Image.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	ImageMemoryBarrier::ImageMemoryBarrier(const Image& image, const VkImageAspectFlags aspect)
		: mImageMemoryBarrier
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_NONE,
			.dstAccessMask = VK_ACCESS_NONE,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image,
			.subresourceRange = VkImageSubresourceRange{
				.aspectMask = aspect,
				.baseMipLevel = 0,
				.levelCount = VK_REMAINING_MIP_LEVELS,
				.baseArrayLayer = 0,
				.layerCount = VK_REMAINING_ARRAY_LAYERS
			},
		}
	{}


	ImageMemoryBarrier::operator VkImageMemoryBarrier() const
	{
		return mImageMemoryBarrier;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::WithSrcAccessMask(VkAccessFlags accessMask)
	{
		mImageMemoryBarrier.srcAccessMask = accessMask;
		return *this;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::WithDstAccessMask(VkAccessFlags accessMask)
	{
		mImageMemoryBarrier.dstAccessMask = accessMask;
		return *this;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::FromLayout(VkImageLayout layout)
	{
		mImageMemoryBarrier.oldLayout = layout;
		return *this;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::ToLayout(VkImageLayout layout)
	{
		mImageMemoryBarrier.newLayout = layout;
		return *this;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::SrcQueueFamily(uint32_t queueFamily)
	{
		mImageMemoryBarrier.srcQueueFamilyIndex = queueFamily;
		return *this;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::DstQueueFamily(uint32_t queueFamily)
	{
		mImageMemoryBarrier.dstQueueFamilyIndex = queueFamily;
		return *this;
	}


	ImageMemoryBarrier& ImageMemoryBarrier::WithSubresourceRange(VkImageSubresourceRange range)
	{
		mImageMemoryBarrier.subresourceRange = range;
		return *this;
	}
}
