//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Swapchain.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Device.hpp"
#include "Queue.hpp"
// Standard Library
#include <memory>

//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	Swapchain::Swapchain(const Device& device, const Surface& surface, Core::Math::Vec2i extents)
		: mDevice(device)
		  , mSize(extents)
		  , mNextImageFence(device.Create<Fence>())
		  , mNextImageIndex([this]() { return CalculateNextImageIndex(); })
		  , mNextImage([this]() { return CalculateNextImage(); })
	{
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device.mPhysicalDevice, surface.mSurface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device.mPhysicalDevice, surface.mSurface, &formatCount, formats.data());
		mFormat = formats[0];

		VkSwapchainCreateInfoKHR createInfo
			{
				.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.pNext = nullptr,
				.flags = 0,
				.surface = surface.mSurface,
				.minImageCount = 2,
				.imageFormat = mFormat.format,
				.imageColorSpace = formats[0].colorSpace,
				.imageExtent = VkExtent2D {.width = static_cast<uint32_t>(mSize[0]), .height = static_cast<uint32_t>(mSize[1])},
				.imageArrayLayers = 1,
				.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
				.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices = nullptr,
				.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
				.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				.presentMode = VK_PRESENT_MODE_FIFO_KHR,
				.clipped = VK_TRUE,
				.oldSwapchain = VK_NULL_HANDLE,
			};

		Core::AssertEQ(vkCreateSwapchainKHR(mDevice->mDevice, &createInfo, nullptr, &mSwapchain), VK_SUCCESS);
	}


	Swapchain::Swapchain(Swapchain&& rhs) noexcept
		: mSwapchain(std::exchange(rhs.mSwapchain, nullptr))
		  , mDevice(std::move(rhs.mDevice))
		  , mNextImageFence(std::move(rhs.mNextImageFence))
		  , mSize(std::exchange(rhs.mSize, {}))
		  , mFormat(std::exchange(rhs.mFormat, VkSurfaceFormatKHR {}))
		  , mNextImageIndex(rhs.mNextImageIndex)
		  , mNextImage(rhs.mNextImage) {}


	Swapchain& Swapchain::operator=(Swapchain&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Swapchain::~Swapchain()
	{
		if (mSwapchain)
		{
			vkDestroySwapchainKHR(mDevice->mDevice, mSwapchain, nullptr);
		}
	}


	Core::Math::Vec2i Swapchain::GetSize() const
	{
		return mSize;
	}


	VkSurfaceFormatKHR Swapchain::GetSurfaceFormat() const
	{
		return mFormat;
	}


	uint32_t Swapchain::GetNextImageIndex()
	{
		return mNextImageIndex.Get();
	}


	VkImage Swapchain::GetNextImage()
	{
		return mNextImage.Get();
	}


	void Swapchain::Present(const Queue& queue)
	{
		VkResult result;
		uint32_t imageIndex = GetNextImageIndex();
		VkPresentInfoKHR presentInfo {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.swapchainCount = 1,
			.pSwapchains = &mSwapchain,
			.pImageIndices = &imageIndex,
			.pResults = &result,
		};


		switch (vkQueuePresentKHR(queue.mQueue, &presentInfo))
		{
			{
				case VK_SUCCESS:
				case VK_SUBOPTIMAL_KHR:
					Core::Assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
					mNextImageIndex.Invalidate();
					mNextImage.Invalidate();
					break;
				default:
					Core::Unreachable();
			}
		}
	}


	uint32_t Swapchain::CalculateNextImageIndex()
	{
		Fence nextImageFence = mDevice->Create<Fence>();
		uint32_t imageIndex;
		switch (vkAcquireNextImageKHR(mDevice->mDevice, mSwapchain, UINT64_MAX, VK_NULL_HANDLE, nextImageFence.mFence,
									  &imageIndex))
		{
			case VK_SUCCESS:
			case VK_SUBOPTIMAL_KHR:
				break;
			default:
				Core::Unreachable();
		}
		nextImageFence.Wait();
		return imageIndex;
	}


	VkImage Swapchain::CalculateNextImage()
	{
		uint32_t imageCount;
		Core::AssertEQ(vkGetSwapchainImagesKHR(mDevice->mDevice, mSwapchain, &imageCount, nullptr), VK_SUCCESS);
		std::vector<VkImage> images(imageCount);
		Core::AssertEQ(vkGetSwapchainImagesKHR(mDevice->mDevice, mSwapchain, &imageCount, images.data()), VK_SUCCESS);
		return images[GetNextImageIndex()];
	}
}
