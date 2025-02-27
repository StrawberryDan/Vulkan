//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Swapchain.hpp"
#include "Device.hpp"
#include "Queue.hpp"
#include "CommandBuffer.hpp"
#include "Framebuffer.hpp"
#include "Surface.hpp"
// Strawberry Window
#include "Strawberry/Window/Window.hpp"
// Standard Library
#include <algorithm>
#include <memory>

//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	Swapchain::Swapchain(const Queue& queue, const Surface& surface, Core::Math::Vec2i extents, VkPresentModeKHR presentMode)
		: mQueue(queue)
		, mSize(extents)
	{
		auto surfaceCapabilities = surface.GetCapabilities();
		mSize[0]                 = std::clamp<int>(mSize[0], surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		mSize[1]                 = std::clamp<int>(mSize[1], surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);


		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mQueue->GetDevice()->GetPhysicalDevice().mPhysicalDevice, surface.mSurface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> deviceFormats(formatCount);;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mQueue->GetDevice()->GetPhysicalDevice().mPhysicalDevice,
		                                     surface.mSurface,
		                                     &formatCount,
		                                     deviceFormats.data());

		std::erase_if(deviceFormats,
		              [&](const VkSurfaceFormatKHR& x) -> bool
		              {
			              const bool colorspace = x.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR;
			              const bool format     = x.format == VK_FORMAT_B8G8R8A8_SRGB;
			              return !(format && colorspace);
		              });

		std::sort(deviceFormats.begin(),
		          deviceFormats.end(),
		          [](const VkSurfaceFormatKHR& a, const VkSurfaceFormatKHR& b)
		          {
			          return a.format > b.format;
		          });

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
			.imageExtent = VkExtent2D{.width = static_cast<uint32_t>(mSize[0]), .height = static_cast<uint32_t>(mSize[1])},
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE,
		};

		Core::AssertEQ(vkCreateSwapchainKHR(*queue.GetDevice(), &createInfo, nullptr, &mSwapchain), VK_SUCCESS);


		uint32_t imageCount = 0;
		Core::AssertEQ(vkGetSwapchainImagesKHR(*queue.GetDevice(), mSwapchain, &imageCount, nullptr), VK_SUCCESS);
		std::vector<VkImage> imageHandles(imageCount);
		Core::AssertEQ(vkGetSwapchainImagesKHR(*queue.GetDevice(), mSwapchain, &imageCount, imageHandles.data()), VK_SUCCESS);
		for (VkImage handle: imageHandles)
		{
			mImages.emplace_back(*queue.GetDevice(), handle, mSize.AsType<unsigned int>().AppendedWith(1), mFormat.format);
		}
	}


	Swapchain::Swapchain(Swapchain&& rhs) noexcept
		: mSwapchain(std::exchange(rhs.mSwapchain, nullptr))
		, mQueue(std::move(rhs.mQueue))
		, mSize(std::exchange(rhs.mSize, {}))
		, mFormat(std::exchange(rhs.mFormat, VkSurfaceFormatKHR{}))
		, mImages(std::move(rhs.mImages))
		, mNextImageIndex(std::move(rhs.mNextImageIndex)) {}


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
		// Images are destroyed by vkDestorySwapchain, so we should release our hold of them here.
		for (auto& image: mImages)
		{
			image.Release();
		}

		if (mSwapchain)
		{
			vkDestroySwapchainKHR(*mQueue->GetDevice(), mSwapchain, nullptr);
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


	Core::Optional<Image*> Swapchain::GetNextImage()
	{
		auto index = GetNextImageIndex();
		if (!index) return {};
		return &mImages[*index];
	}


	Core::Optional<Image*> Swapchain::WaitForNextImage()
	{
		auto index = WaitForNextImageIndex();
		if (!index) return {};
		return &mImages[*index];
	}


	Core::Optional<uint32_t> Swapchain::GetNextImageIndex()
	{
		if (mNextImageIndex) return *mNextImageIndex;


		Fence    fence(*mQueue->GetDevice());
		uint32_t imageIndex = 0;
		auto     result     = vkAcquireNextImageKHR(*mQueue->GetDevice(), mSwapchain, 0, VK_NULL_HANDLE, fence.mFence, &imageIndex);
		fence.Wait();


		switch (result)
		{
			case VK_SUCCESS:
				mNextImageIndex = imageIndex;
				return imageIndex;
			case VK_TIMEOUT:
			case VK_NOT_READY:
			case VK_SUBOPTIMAL_KHR:
				return {};
			default:
				Core::Unreachable();
		}
	}


	Core::Optional<uint32_t> Swapchain::WaitForNextImageIndex()
	{
		if (mNextImageIndex) return *mNextImageIndex;


		Fence    fence(*mQueue->GetDevice());
		uint32_t imageIndex = 0;
		auto     result     = vkAcquireNextImageKHR(*mQueue->GetDevice(), mSwapchain, 0, VK_NULL_HANDLE, fence.mFence, &imageIndex);
		fence.Wait();

		switch (result)
		{
			case VK_SUCCESS:
				mNextImageIndex = imageIndex;
				return imageIndex;
			case VK_TIMEOUT:
			case VK_NOT_READY:
			case VK_SUBOPTIMAL_KHR:
				return {};
			default:
				Core::Unreachable();
		}
	}


	void Swapchain::Present()
	{
		uint32_t imageIndex = WaitForNextImageIndex().Unwrap();


		VkResult         presentResult;
		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.swapchainCount = 1,
			.pSwapchains = &mSwapchain,
			.pImageIndices = &imageIndex,
			.pResults = &presentResult,
		};


		auto result = vkQueuePresentKHR(mQueue->mQueue, &presentInfo);
		switch (result)
		{
				{
				case VK_SUCCESS:
					break;
				case VK_SUBOPTIMAL_KHR:
					break;
				case VK_ERROR_OUT_OF_DATE_KHR:
					mIsOutOfDate = true;
					break;
				default:
					Core::Unreachable();
				}
		}

		switch (presentResult)
		{
			case VK_SUCCESS:
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				mIsOutOfDate = true;
				break;
			default:
				Core::Unreachable();
		}

		mNextImageIndex.Reset();
	}


	bool Swapchain::IsOutOfDate() const
	{
		return mIsOutOfDate;
	}
}
