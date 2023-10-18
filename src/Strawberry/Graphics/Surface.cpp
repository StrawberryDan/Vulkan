//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Surface.hpp"
#include "Window.hpp"
#include "Instance.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Surface::Surface(const Window::Window& window, const Instance& instance)
		: mInstance(instance.mInstance)
	{
		Core::AssertEQ(glfwCreateWindowSurface(mInstance, window.mHandle, nullptr, &mSurface), GLFW_NO_ERROR);
	}


	Surface::Surface(Surface&& rhs)
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
