//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Window.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"


namespace Strawberry::Graphics
{
	void Window::Initialise()
	{
		Core::Assert(glfwInit() == GLFW_TRUE);
	}


	void Window::Terminate()
	{
		glfwTerminate();
	}


	std::atomic<unsigned int> Window::sInstanceCount = 0;


	Window::Window(const std::string& title, Core::Math::Vec2i size)
	{
		if (sInstanceCount++ == 0) Initialise();

		Core::Assert(size[0] > 0 && size[1] > 0);
		mHandle = glfwCreateWindow(size[0], size[1], title.c_str(), nullptr, nullptr);
	}


	Window::Window(Window&& rhs)
		: mHandle(std::exchange(rhs.mHandle, nullptr))
	{}


	Window& Window::operator=(Window&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Window::~Window()
	{
		Core::Assert(sInstanceCount > 0);
		glfwDestroyWindow(mHandle);

		if (--sInstanceCount == 0) Terminate();
	}
}