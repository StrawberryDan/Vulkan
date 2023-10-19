//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Image.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <Strawberry/Core/Math/Vector.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Image::Image(const Device& device, uint32_t extent, VkFormat format, VkImageUsageFlags usage,
				 uint32_t mipLevels, uint32_t arrayLayers, VkImageTiling tiling, VkImageLayout initialLayout)
		: mImage(nullptr)
		  , mDevice(device.mDevice)
	{
		VkImageCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_1D,
			.format = format,
			.extent = VkExtent3D{extent, 1, 1},
			.mipLevels = mipLevels,
			.arrayLayers = arrayLayers,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = tiling,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = initialLayout
		};

		Core::AssertEQ(vkCreateImage(mDevice, &createInfo, nullptr, &mImage), VK_SUCCESS);
	}


	Image::Image(const Device& device, Core::Math::Vec2i extent, VkFormat format, VkImageUsageFlags usage,
				 uint32_t mipLevels, uint32_t arrayLayers, VkImageTiling tiling, VkImageLayout initialLayout)
		: mImage(nullptr)
		  , mDevice(device.mDevice)
	{
		VkImageCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = VkExtent3D{static_cast<uint32_t>(extent[0]), static_cast<uint32_t>(extent[1]), 1},
			.mipLevels = mipLevels,
			.arrayLayers = arrayLayers,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = tiling,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = initialLayout
		};

		Core::AssertEQ(vkCreateImage(mDevice, &createInfo, nullptr, &mImage), VK_SUCCESS);
	}


	Image::Image(const Device& device, Core::Math::Vec3i extent, VkFormat format, VkImageUsageFlags usage,
				 uint32_t mipLevels, uint32_t arrayLayers, VkImageTiling tiling, VkImageLayout initialLayout)
		: mImage(nullptr)
		, mDevice(device.mDevice)
	{
		VkImageCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_3D,
			.format = format,
			.extent = VkExtent3D{static_cast<uint32_t>(extent[0]), static_cast<uint32_t>(extent[1]), static_cast<uint32_t>(extent[2])},
			.mipLevels = mipLevels,
			.arrayLayers = arrayLayers,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = tiling,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = initialLayout
		};

		Core::AssertEQ(vkCreateImage(mDevice, &createInfo, nullptr, &mImage), VK_SUCCESS);
	}


	Image::Image(Image&& rhs) noexcept
		: mImage(std::exchange(rhs.mImage, nullptr))
		  , mDevice(std::exchange(rhs.mDevice, nullptr)) {}


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
			vkDestroyImage(mDevice, mImage, nullptr);
		}
	}
}
