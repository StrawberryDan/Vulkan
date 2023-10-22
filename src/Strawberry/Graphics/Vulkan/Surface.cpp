//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Surface.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Instance.hpp"
#include "Device.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	Surface::Surface(const Window::Window& window, const Device& device)
		: mInstance(device.mInstance)
		, mDevice(device.mDevice)
	{
		Core::AssertEQ(glfwCreateWindowSurface(mInstance, window.mHandle, nullptr, &mSurface), GLFW_NO_ERROR);
	}


	Surface::Surface(Surface&& rhs) noexcept
		: mSurface(std::exchange(rhs.mSurface, nullptr))
		, mInstance(std::exchange(rhs.mInstance, nullptr))
	{}


	Surface& Surface::operator=(Surface&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Surface::~Surface()
	{
		if (mSurface)
		{
			vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		}
	}
}
