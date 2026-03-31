#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/MemoryTypeCriteria.hpp"
#include "Strawberry/Vulkan/Memory/MemoryBlock.hpp"
#include "Strawberry/Vulkan/Memory/Allocator/MonoAllocator.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>


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
		friend class Swapchain;

	public:
		class Builder
		{
		public:
			Builder(Device& device, MemoryTypeCriteria memoryTypeCriteria);


			Builder(MemoryBlock&& allocation)
				: mAllocationSource(std::move(allocation)) {}


			Builder(MonoAllocator& allocator)
				: mAllocationSource(&allocator) {}


			Builder(PolyAllocator& allocator, MemoryTypeCriteria memoryTypeCriteria)
				: mAllocationSource(&allocator)
				  , mMemoryTypeCriteria(memoryTypeCriteria) {}


			Builder&& WithExtent(unsigned extent);

			Builder&& WithExtent(Core::Math::Vec2u extent);

			Builder&& WithExtent(Core::Math::Vec3u extent);

			Builder&& WithFormat(VkFormat format);

			Builder&& WithUsage(VkImageUsageFlags usage);

			Builder&& WithSamples(VkSampleCountFlagBits samples);

			Builder&& WithMipLevels(uint32_t mipLevels);

			Builder&& WithArrayLayers(size_t arrayLayers);

			Builder&& WithTiling(VkImageTiling tiling);

			Builder&& WithInitialLayout(VkImageLayout layout);


			Image Build();

		private:
			const Device& GetDevice() const;


			mutable Core::Variant<MemoryBlock, MonoAllocator*, PolyAllocator*> mAllocationSource;
			MemoryTypeCriteria                                                   mMemoryTypeCriteria;


			Core::Optional<VkImageType>       mImageType;
			Core::Optional<Core::Math::Vec3u> mExtent;
			Core::Optional<VkFormat>          mFormat;
			Core::Optional<VkImageUsageFlags> mUsage;
			VkSampleCountFlagBits             mSamples       = VK_SAMPLE_COUNT_1_BIT;
			uint32_t                          mMipLevels     = 1;
			size_t                            mArrayLayers   = 1;
			VkImageTiling                     mTiling        = VK_IMAGE_TILING_OPTIMAL;
			VkImageLayout                     mInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};

		using Handle = VkImage;

		Image(const Image& rhs) = delete;

		Image& operator=(const Image& rhs) = delete;

		Image(Image&& rhs) noexcept;

		Image& operator=(Image&& rhs) noexcept;

		~Image();


		VkImage Release();


		Handle GetHandle() const
		{
			return mImage;
		}


		operator VkImage() const
		{
			return GetHandle();
		}


		[[nodiscard]] const Device& GetDevice() const;

		[[nodiscard]] VkFormat GetFormat() const;

		[[nodiscard]] Core::Math::Vec3u GetSize() const;

		[[nodiscard]] VkSampleCountFlagBits GetSamples() const;

		[[nodiscard]] unsigned int GetArrayLayerCount() const;

	private:
		Image(VkImage           imageHandle,
			  MemoryBlock&&      allocation,
			  Core::Math::Vec3u extent,
			  VkFormat          format);

		Image(VkImage           imageHandle,
			  Core::Math::Vec3u extent,
			  VkFormat          format);


		VkImage           mImage;
		MemoryBlock        mMemory;
		VkFormat          mFormat;
		Core::Math::Vec3u mExtent;
		VkSampleCountFlagBits mSamples = VK_SAMPLE_COUNT_1_BIT;
		unsigned int mArrayLayerCount = 1;
	};
}
