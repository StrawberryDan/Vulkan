//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Swapchain.hpp"
#include "Window.hpp"
#include "Device.hpp"
// Standard Library
#include <memory>

//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Swapchain::Swapchain(const Device& device, const Surface& surface, Core::Math::Vec2i extents)
		: mDevice(device.mDevice)
	{
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device.mPhysicalDevice, surface.mSurface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device.mPhysicalDevice, surface.mSurface, &formatCount, formats.data());

		VkSwapchainCreateInfoKHR createInfo
		{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = surface.mSurface,
			.minImageCount = 2,
			.imageFormat = formats[0].format,
			.imageColorSpace = formats[0].colorSpace,
			.imageExtent = VkExtent2D{.width = static_cast<uint32_t>(extents[0]), .height = static_cast<uint32_t>(extents[1])},
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

		Core::AssertEQ(vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapchain), VK_SUCCESS);
	}


	Swapchain::Swapchain(Swapchain&& rhs) noexcept
		: mSwapchain(std::exchange(rhs.mSwapchain, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
	{}


	Swapchain& Swapchain::operator=(Swapchain&& rhs)
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
			vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
		}
	}
}
