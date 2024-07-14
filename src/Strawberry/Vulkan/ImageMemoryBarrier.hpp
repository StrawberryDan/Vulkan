#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <vulkan/vulkan.h>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Image;


	class ImageMemoryBarrier
	{
		public:
			ImageMemoryBarrier(const Image& image, const VkImageAspectFlags aspect);


			operator VkImageMemoryBarrier() const;


			ImageMemoryBarrier& WithSrcAccessMask(VkAccessFlags accessMask);
			ImageMemoryBarrier& WithDstAccessMask(VkAccessFlags accessMask);
			ImageMemoryBarrier& FromLayout(VkImageLayout layout);
			ImageMemoryBarrier& ToLayout(VkImageLayout layout);
			ImageMemoryBarrier& SrcQueueFamily(uint32_t queueFamily);
			ImageMemoryBarrier& DstQueueFamily(uint32_t queueFamily);
			ImageMemoryBarrier& WithSubresourceRange(VkImageSubresourceRange range);

		private:
			VkImageMemoryBarrier mImageMemoryBarrier;
	};
}
