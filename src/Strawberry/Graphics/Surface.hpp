#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	namespace Window { class Window; }
	class Device;


	class Surface
	{
		friend class Swapchain;


	public:
		Surface(const Window::Window& window, const Device& device);
		Surface(const Surface& rhs) = delete;
		Surface& operator=(const Surface& rhs) = delete;
		Surface(Surface&& rhs);
		Surface& operator=(Surface&& rhs);
		~Surface();

	private:
		VkSurfaceKHR mSurface;

		VkInstance mInstance;
		VkDevice mDevice;
	};
}
