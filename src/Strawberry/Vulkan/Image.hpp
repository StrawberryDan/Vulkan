#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Memory/Allocator.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;
	class Queue;


	class Image
			: public Core::EnableReflexivePointer
	{
		friend class ImageView;
		friend class CommandBuffer;

	public:
		Image(Allocator*                allocator,
		      const MemoryTypeCriteria& memoryCriteria,
		      uint32_t                  extent,
		      VkFormat                  format,
		      VkImageUsageFlags         usage,
		      uint32_t                  mipLevels     = 1,
		      uint32_t                  arrayLayers   = 1,
		      VkImageTiling             tiling        = VK_IMAGE_TILING_OPTIMAL,
		      VkImageLayout             initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;

		Image(Allocator*                allocator,
		      const MemoryTypeCriteria& memoryCriteria,
		      Core::Math::Vec2u         extent,
		      VkFormat                  format,
		      VkImageUsageFlags         usage,
		      uint32_t                  mipLevels     = 1,
		      uint32_t                  arrayLayers   = 1,
		      VkImageTiling             tiling        = VK_IMAGE_TILING_OPTIMAL,
		      VkImageLayout             initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;

		Image(Allocator*                allocator,
		      const MemoryTypeCriteria& memoryCriteria,
		      Core::Math::Vec3u         extent,
		      VkFormat                  format,
		      VkImageUsageFlags         usage,
		      uint32_t                  mipLevels     = 1,
		      uint32_t                  arrayLayers   = 1,
		      VkImageTiling             tiling        = VK_IMAGE_TILING_OPTIMAL,
		      VkImageLayout             initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;

		Image(const Device&     device,
		      VkImage           imageHandle,
		      Core::Math::Vec3u extent,
		      VkFormat          format);

		Image(const Image& rhs)            = delete;
		Image& operator=(const Image& rhs) = delete;
		Image(Image&& rhs) noexcept;
		Image& operator=(Image&& rhs) noexcept;
		~Image();


		template<std::movable T, typename... Args>
		T Create(const Args&... args) const
		{
			return T(*this, std::forward<const Args&>(args)...);
		}


		VkImage Release();


		operator VkImage() const
		{
			return mImage;
		}


		[[nodiscard]] VkFormat          GetFormat() const;
		[[nodiscard]] Core::Math::Vec3u GetSize() const;

	private:
		VkImage           mImage;
		AllocationView    mMemory;
		VkDevice          mDevice;
		VkFormat          mFormat;
		Core::Math::Vec3u mSize;
	};
}
