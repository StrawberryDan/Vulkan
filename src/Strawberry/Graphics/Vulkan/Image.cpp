//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Image.hpp"
#include "Device.hpp"
#include "Queue.hpp"
#include "CommandBuffer.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <Strawberry/Core/Math/Vector.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	Image::Image(const Device& device, uint32_t extent, VkFormat format, VkImageUsageFlags usage,
				 uint32_t mipLevels, uint32_t arrayLayers, VkImageTiling tiling, VkImageLayout initialLayout)
		: mImage(nullptr)
		, mDevice(device.mDevice)
		, mSize(static_cast<int>(extent), 0, 0)
		, mLastRecordedLayout(initialLayout)
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


	Image::Image(const Device& device, Core::Math::Vec2u extent, VkFormat format, VkImageUsageFlags usage,
				 uint32_t mipLevels, uint32_t arrayLayers, VkImageTiling tiling, VkImageLayout initialLayout)
		: mImage(nullptr)
		, mDevice(device.mDevice)
		, mSize(extent)
		, mLastRecordedLayout(initialLayout)
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


		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(mDevice, mImage, &memoryRequirements);
		mMemory = device.Create<DeviceMemory>(memoryRequirements.size, memoryRequirements.memoryTypeBits,
											  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		Core::AssertEQ(vkBindImageMemory(mDevice, mImage, mMemory.mDeviceMemory, 0), VK_SUCCESS);
	}


	Image::Image(const Device& device, Core::Math::Vec3u extent, VkFormat format, VkImageUsageFlags usage,
				 uint32_t mipLevels, uint32_t arrayLayers, VkImageTiling tiling, VkImageLayout initialLayout)
		: mImage(nullptr)
		, mDevice(device.mDevice)
		, mSize(extent)
		, mLastRecordedLayout(initialLayout)
	{
		VkImageCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_3D,
			.format = format,
			.extent = VkExtent3D{static_cast<uint32_t>(extent[0]), static_cast<uint32_t>(extent[1]),
								 static_cast<uint32_t>(extent[2])},
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


		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(mDevice, mImage, &memoryRequirements);

		mMemory = device.Create<DeviceMemory>(memoryRequirements.size, memoryRequirements.memoryTypeBits);
		Core::AssertEQ(vkBindImageMemory(mDevice, mImage, mMemory.mDeviceMemory, 0), VK_SUCCESS);
	}


	Image::Image(Image&& rhs) noexcept
		: mImage(std::exchange(rhs.mImage, nullptr))
		, mMemory(std::move(rhs.mMemory))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
		, mSize(std::exchange(rhs.mSize, Core::Math::Vec3u()))
		, mLastRecordedLayout(std::exchange(rhs.mLastRecordedLayout, VK_IMAGE_LAYOUT_UNDEFINED))
	{

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
			vkDestroyImage(mDevice, mImage, nullptr);
		}
	}


	Core::Math::Vec3u Image::GetSize() const
	{
		return mSize;
	}


	void Image::ClearColor(Queue& queue, Core::Math::Vec4f clearColor)
	{
		auto commandBuffer = queue.Create<CommandBuffer>();
		commandBuffer.Begin(true);
		commandBuffer.ClearColorImage(*this, clearColor);
		commandBuffer.End();
		queue.Submit(commandBuffer);
	}
}
