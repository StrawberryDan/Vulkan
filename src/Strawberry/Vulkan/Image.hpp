#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Memory/Allocator.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Overload.hpp"
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
		class Builder
		{
		public:
			Builder(SingleAllocator& allocator)
				: allocator(allocator)
			{}


			Image Build()
			{
				return extent->Visit(Core::Overload(
					[&](unsigned x)
					{
						return Image(*allocator, x, format.Unwrap(), usage.Unwrap(), mipLevels, arrayLayers, tiling, initialLayout);
					},
					[&](Core::Math::Vec2u dims)
					{
						return Image(*allocator, dims, format.Unwrap(), usage.Unwrap(), mipLevels, arrayLayers, tiling, initialLayout);
					},
					[&](Core::Math::Vec3u dims)
					{
						return Image(*allocator, dims, format.Unwrap(), usage.Unwrap(), mipLevels, arrayLayers, tiling, initialLayout);
					}
				));
			}


			Builder&& WithExtent(unsigned extent)
			{
				this->extent = extent;
				return std::move(*this);
			}


			Builder&& WithExtent(Core::Math::Vec2u extent)
			{
				this->extent = extent;
				return std::move(*this);
			}


			Builder&& WithExtent(Core::Math::Vec3u extent)
			{
				this->extent = extent;
				return std::move(*this);
			}


			Builder&& WithFormat(VkFormat format)
			{
				this->format = format;
				return std::move(*this);
			}


			Builder&& WithUsage(VkImageUsageFlags usage)
			{
				this->usage = usage;
				return std::move(*this);
			}


			Builder&& WithMipLevels(uint32_t mipLevels)
			{
				this->mipLevels = mipLevels;
				return std::move(*this);
			}


			Builder&& WithArrayLayers(uint32_t arrayLayers)
			{
				this->arrayLayers = arrayLayers;
				return std::move(*this);
			}


			Builder&& WithTiling(VkImageTiling tiling)
			{
				this->tiling = tiling;
				return std::move(*this);
			}


			Builder&& WithInitialLayout(VkImageLayout layout)
			{
				this->initialLayout = layout;
				return std::move(*this);
			}

		private:
			Core::ReflexivePointer<SingleAllocator>                                             allocator;
			Core::Optional<Core::Variant<unsigned, Core::Math::Vec2u, Core::Math::Vec3u>> extent;
			Core::Optional<VkFormat>                                                      format;
			Core::Optional<VkImageUsageFlags>                                             usage;
			uint32_t                                                                      mipLevels     = 1;
			uint32_t                                                                      arrayLayers   = 1;
			VkImageTiling                                                                 tiling        = VK_IMAGE_TILING_OPTIMAL;
			VkImageLayout                                                                 initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};


		Image(SingleAllocator&        allocator,
		      uint32_t          extent,
		      VkFormat          format,
		      VkImageUsageFlags usage,
		      uint32_t          mipLevels     = 1,
		      uint32_t          arrayLayers   = 1,
		      VkImageTiling     tiling        = VK_IMAGE_TILING_OPTIMAL,
		      VkImageLayout     initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;

		Image(SingleAllocator&        allocator,
		      Core::Math::Vec2u extent,
		      VkFormat          format,
		      VkImageUsageFlags usage,
		      uint32_t          mipLevels     = 1,
		      uint32_t          arrayLayers   = 1,
		      VkImageTiling     tiling        = VK_IMAGE_TILING_OPTIMAL,
		      VkImageLayout     initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;

		Image(SingleAllocator&        allocator,
		      Core::Math::Vec3u extent,
		      VkFormat          format,
		      VkImageUsageFlags usage,
		      uint32_t          mipLevels     = 1,
		      uint32_t          arrayLayers   = 1,
		      VkImageTiling     tiling        = VK_IMAGE_TILING_OPTIMAL,
		      VkImageLayout     initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;

		Image(const Device&     device,
		      VkImage           imageHandle,
		      Core::Math::Vec3u extent,
		      VkFormat          format);

		Image(const Image& rhs)            = delete;
		Image& operator=(const Image& rhs) = delete;
		Image(Image&& rhs) noexcept;
		Image& operator=(Image&& rhs) noexcept;
		~Image();


		VkImage Release();


		operator VkImage() const
		{
			return mImage;
		}


		[[nodiscard]] VkFormat          GetFormat() const;
		[[nodiscard]] Core::Math::Vec3u GetSize() const;

	private:
		VkImage           mImage;
		Allocation        mMemory;
		VkDevice          mDevice;
		VkFormat          mFormat;
		Core::Math::Vec3u mSize;
	};
}
