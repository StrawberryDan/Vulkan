#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
#include <Strawberry/Core/Math/Vector.hpp>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;


	class Image
	{
		friend class ImageView;


	public:
		Image(const Device& device,
			  uint32_t extent,
			  VkFormat format,
			  VkImageUsageFlags usage,
			  uint32_t mipLevels = 1,
			  uint32_t arrayLayers = 1,
			  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			  VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

		Image(const Device& device,
			  Core::Math::Vec2i extent,
			  VkFormat format,
			  VkImageUsageFlags usage,
			  uint32_t mipLevels = 1,
			  uint32_t arrayLayers = 1,
			  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			  VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

		Image(const Device& device,
			  Core::Math::Vec3i extent,
			  VkFormat format,
			  VkImageUsageFlags usage,
			  uint32_t mipLevels = 1,
			  uint32_t arrayLayers = 1,
			  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			  VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

		Image(const Image& rhs) = delete;
		Image& operator=(const Image& rhs) = delete;
		Image(Image&& rhs) noexcept ;
		Image& operator=(Image&& rhs) noexcept;
		~Image();


		template <std::movable T, typename... Args>
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		VkImage mImage;
		VkDevice mDevice;
	};
}
