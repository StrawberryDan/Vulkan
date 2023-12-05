//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Swapchain.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Device.hpp"
#include "Queue.hpp"
#include "CommandBuffer.hpp"
#include "Framebuffer.hpp"
// Standard Library
#include <algorithm>
#include <memory>

//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	Swapchain::Swapchain(const Queue& queue, const Surface& surface, Core::Math::Vec2i extents)
		: mQueue(queue)
		, mCommandPool(mQueue->Create<CommandPool>(false))
		, mSize(extents)
		, mNextImageFence(queue.GetDevice()->Create<Fence>())
		, mNextImageIndex([this]() { return CalculateNextImageIndex(); })
		, mNextImage([this]() { return CalculateNextImage(); })
	{
		auto surfaceCapabilities = surface.GetCapabilities();
		mSize[0] = std::clamp<int>(mSize[0], surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		mSize[1] = std::clamp<int>(mSize[1], surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);


		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mQueue->GetDevice()->mPhysicalDevice, surface.mSurface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> deviceFormats(formatCount);;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mQueue->GetDevice()->mPhysicalDevice, surface.mSurface, &formatCount,
											 deviceFormats.data());

		std::erase_if(deviceFormats, [&](const VkSurfaceFormatKHR& x) -> bool
		{
			const bool colorspace = x.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR;
			const bool format = x.format == VK_FORMAT_B8G8R8A8_SRGB;
			return !(format && colorspace);
		});

		std::sort(deviceFormats.begin(), deviceFormats.end(),
				  [](const VkSurfaceFormatKHR& a, const VkSurfaceFormatKHR& b) { return a.format > b.format; });

		mFormat = deviceFormats.at(0);

		VkSwapchainCreateInfoKHR createInfo
			{
				.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.pNext = nullptr,
				.flags = 0,
				.surface = surface.mSurface,
				.minImageCount = 2,
				.imageFormat = mFormat.format,
				.imageColorSpace = deviceFormats[0].colorSpace,
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

		Core::AssertEQ(vkCreateSwapchainKHR(queue.GetDevice()->mDevice, &createInfo, nullptr, &mSwapchain), VK_SUCCESS);
	}


	Swapchain::Swapchain(Swapchain&& rhs) noexcept
		: mSwapchain(std::exchange(rhs.mSwapchain, nullptr))
		  , mQueue(std::move(rhs.mQueue))
		  , mCommandPool(std::move(rhs.mCommandPool))
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
			vkDestroySwapchainKHR(mQueue->GetDevice()->mDevice, mSwapchain, nullptr);
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


	void Swapchain::Present()
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


		result = vkQueuePresentKHR(mQueue->mQueue, &presentInfo);
		switch (result)
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


	void Swapchain::Present(Framebuffer& framebuffer)
	{
		CommandBuffer buffer = mCommandPool.Create<CommandBuffer>();
		buffer.Begin(true);

		for (int i = 0; i < framebuffer.GetColorAttachmentCount(); i++)
		{
			buffer.CopyImageToSwapchain(framebuffer.GetColorAttachment(i), *this);
		}

		buffer.ImageMemoryBarrier(GetNextImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		buffer.End();
		mQueue->Submit(std::move(buffer));
		mQueue->Wait();


		Present();
	}


	uint32_t Swapchain::CalculateNextImageIndex()
	{
		Fence nextImageFence = mQueue->GetDevice()->Create<Fence>();
		uint32_t imageIndex;
		switch (vkAcquireNextImageKHR(mQueue->GetDevice()->mDevice, mSwapchain, UINT64_MAX, VK_NULL_HANDLE, nextImageFence.mFence,
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
		Core::AssertEQ(vkGetSwapchainImagesKHR(mQueue->GetDevice()->mDevice, mSwapchain, &imageCount, nullptr), VK_SUCCESS);
		std::vector<VkImage> images(imageCount);
		Core::AssertEQ(vkGetSwapchainImagesKHR(mQueue->GetDevice()->mDevice, mSwapchain, &imageCount, images.data()), VK_SUCCESS);
		return images[GetNextImageIndex()];
	}
}
