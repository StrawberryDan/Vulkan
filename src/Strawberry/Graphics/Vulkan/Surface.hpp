#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>

#include "Strawberry/Core/Types/ReflexivePointer.hpp"


//======================================================================================================================
//  Foreward Declarations
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Window { class Window; }


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;


	class Surface
	{
		friend class Swapchain;


	public:
		explicit Surface(const Window::Window& window, const Device& device);
		Surface(const Surface& rhs) = delete;
		Surface& operator=(const Surface& rhs) = delete;
		Surface(Surface&& rhs) noexcept ;
		Surface& operator=(Surface&& rhs);
		~Surface();


		VkSurfaceCapabilitiesKHR GetCapabilities() const;


	private:
		VkSurfaceKHR mSurface;

		VkInstance mInstance;
		Core::ReflexivePointer<Device> mDevice;
	};
}
