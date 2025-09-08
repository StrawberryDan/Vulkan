//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Resource/Image.hpp"
#include "Strawberry/Vulkan/Device/Device.hpp"
#include "Strawberry/Vulkan/Queue/CommandBuffer.hpp"
#include "Strawberry/Vulkan/Resource/Buffer.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <Strawberry/Core/Math/Vector.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Image::Builder::Builder(Device& device, MemoryTypeCriteria memoryTypeCriteria)
		: Builder(device.GetAllocator(), memoryTypeCriteria)
	{

	}

	Image::Builder&& Image::Builder::WithExtent(unsigned extent)
	{
		this->mImageType = VK_IMAGE_TYPE_1D;
		this->mExtent = Core::Math::Vec3u(extent, 1, 1);
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithExtent(Core::Math::Vec2u extent)
	{
		this->mImageType = VK_IMAGE_TYPE_2D;
		this->mExtent = Core::Math::Vec3u(extent[0], extent[1], 1);
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithExtent(Core::Math::Vec3u extent)
	{
		this->mImageType = VK_IMAGE_TYPE_3D;
		this->mExtent = extent;
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithFormat(VkFormat format)
	{
		this->mFormat = format;
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithUsage(VkImageUsageFlags usage)
	{
		this->mUsage = usage;
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithMipLevels(uint32_t mipLevels)
	{
		this->mMipLevels = mipLevels;
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithArrayLayers(uint32_t arrayLayers)
	{
		this->mArrayLayers = arrayLayers;
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithTiling(VkImageTiling tiling)
	{
		this->mTiling = tiling;
		return std::move(*this);
	}


	Image::Builder&& Image::Builder::WithInitialLayout(VkImageLayout layout)
	{
		this->mInitialLayout = layout;
		return std::move(*this);
	}


	Image Image::Builder::Build()
	{
		const Device& device = GetDevice();

		VkImage imageHandle = VK_NULL_HANDLE;;

		VkImageCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = mImageType.Value(),
			.format = mFormat.Value(),
			.extent = VkExtent3D{
				static_cast<uint32_t>(mExtent.Value()[0]),
				static_cast<uint32_t>(mExtent.Value()[1]),
				static_cast<uint32_t>(mExtent.Value()[2])
			},
			.mipLevels = mMipLevels,
			.arrayLayers = mArrayLayers,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = mTiling,
			.usage = mUsage.Value(),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = mInitialLayout
		};
		Core::AssertEQ(vkCreateImage(device, &createInfo, nullptr, &imageHandle), VK_SUCCESS);


		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(device, imageHandle, &memoryRequirements);
		MemoryBlock memory = mAllocationSource.Visit(
			[&](MemoryBlock& allocation) { return AllocationResult(std::move(allocation)); },
			[&](SingleAllocator* allocator) { return allocator->Allocate(memoryRequirements); },
			[&](MultiAllocator* allocator) { return allocator->Allocate(memoryRequirements, mMemoryTypeCriteria); }
		).Unwrap();

		Core::AssertEQ(vkBindImageMemory(device, imageHandle, memory.Memory(), memory.Offset()), VK_SUCCESS);


		return Image(imageHandle, std::move(memory), mExtent.Value(), mFormat.Value());
	}

	const Device& Image::Builder::GetDevice() const
	{
		return mAllocationSource.Visit(
			[&](const MemoryBlock& allocation) -> const Device&
			{
				return allocation.GetDevice();
			},
			[&](const SingleAllocator* allocator) -> const Device&
			{
				return allocator->GetDevice();
			},
			[&](const MultiAllocator* allocator) -> const Device&
			{
				return allocator->GetDevice();
			}
		);
	}


	Image& Image::operator=(Image&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Image::~Image()
	{
		if (mImage)
		{
			vkDestroyImage(GetDevice(), mImage, nullptr);
		}
	}


	VkImage Image::Release()
	{
		return std::exchange(mImage, VK_NULL_HANDLE);
	}


	const Device& Image::GetDevice() const
	{
		return mMemory.GetDevice();
	}

	VkFormat Image::GetFormat() const
	{
		return mFormat;
	}


	Core::Math::Vec3u Image::GetSize() const
	{
		return mExtent;
	}

	Image::Image(VkImage imageHandle, MemoryBlock &&allocation, Core::Math::Vec3u extent, VkFormat format)
		: mImage(imageHandle)
		, mMemory(std::move(allocation))
		, mFormat(format)
		, mExtent(extent)
	{}

	Image::Image(VkImage imageHandle, Core::Math::Vec3u extent, VkFormat format)
		: mImage(imageHandle)
		, mFormat(format)
		, mExtent(extent) {}


	Image::Image(Image&& rhs) noexcept
		: EnableReflexivePointer(std::move(rhs))
		, mImage(std::exchange(rhs.mImage, nullptr))
		, mMemory(std::move(rhs.mMemory))
		, mFormat(std::exchange(rhs.mFormat, VK_FORMAT_MAX_ENUM))
		, mExtent(std::exchange(rhs.mExtent, Core::Math::Vec3u())) {}
}
