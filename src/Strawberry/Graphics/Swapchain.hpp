#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	namespace Window { class Window; }


	class Device;
	class Surface;


	class Swapchain
	{
	public:
		explicit Swapchain(const Device& device, const Surface& surface, Core::Math::Vec2i extents);
		Swapchain(const Swapchain& rhs) = delete;
		Swapchain& operator=(const Swapchain& rhs) = delete;
		Swapchain(Swapchain&& rhs) noexcept;
		Swapchain& operator=(Swapchain&& rhs);
		~Swapchain();


	private:
		VkSwapchainKHR mSwapchain;

		VkDevice mDevice;
	};
}
