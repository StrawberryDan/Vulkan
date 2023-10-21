//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Window.hpp"
#include "Instance.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// GLFW3
#include "GLFW/glfw3.h"


namespace Strawberry::Graphics::Window
{
	void Window::Initialise()
	{
		Core::Assert(glfwInit() == GLFW_TRUE);
		Core::Assert(glfwVulkanSupported());
	}


	void Window::Terminate()
	{
		glfwTerminate();
	}


	std::atomic<unsigned int> Window::sInstanceCount = 0;


	Core::Mutex<std::map<GLFWwindow*, Window*>> Window::sInstanceMap;


	Window::Window(const std::string& title, Core::Math::Vec2i size)
	{
		if (sInstanceCount++ == 0) Initialise();

		Core::Assert(size[0] > 0 && size[1] > 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		mHandle = glfwCreateWindow(size[0], size[1], title.c_str(), nullptr, nullptr);
		glfwSetKeyCallback(mHandle, &Window::OnKeyEvent);
		glfwSetCharCallback(mHandle, &Window::OnTextEvent);

		sInstanceMap.Lock()->emplace(mHandle, this);
	}


	Window::Window(Window&& rhs) noexcept
		: mHandle(std::exchange(rhs.mHandle, nullptr))
	{
		sInstanceMap.Lock()->insert_or_assign(mHandle, this);
	}


	Window& Window::operator=(Window&& rhs) noexcept
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

		if (mHandle)
		{
			glfwDestroyWindow(mHandle);
			sInstanceMap.Lock()->erase(mHandle);
		}

		if (--sInstanceCount == 0) Terminate();
	}


	Core::Optional<Event> Window::NextEvent()
	{
		if (mEventQueue.empty())
		{
			return {};
		}
		else
		{
			auto event(mEventQueue.front());
			mEventQueue.pop_front();
			return event;
		}
	}


	bool Window::CloseRequested() const
	{
		return glfwWindowShouldClose(mHandle);
	}


	void Window::SwapBuffers()
	{
		glfwSwapInterval(1);
		glfwSwapBuffers(mHandle);
	}


	Core::Math::Vec2i Window::GetSize() const
	{
		Core::Math::Vec2i size;
		glfwGetWindowSize(mHandle, &size[0], &size[1]);
		return size;
	}


	void Window::OnKeyEvent(GLFWwindow* windowHandle, int key, int scancode, int action, int mods)
	{
		Window* window = sInstanceMap.Lock()->at(windowHandle);

		auto GetAction = [](int action)
		{
			switch (action)
			{
				case GLFW_PRESS:
					return Input::KeyAction::Press;
				case GLFW_RELEASE:
					return Input::KeyAction::Release;
				case GLFW_REPEAT:
					return Input::KeyAction::Repeat;
				default:
					Core::Unreachable();
			}
		};

		auto GetModifier = [](int modifier)
		{
			Input::Modifiers result = 0;
			if (modifier & GLFW_MOD_SHIFT) result = result | (Input::Modifiers) Input::Modifier::SHIFT;
			if (modifier & GLFW_MOD_CONTROL) result = result | (Input::Modifiers) Input::Modifier::CTRL;
			if (modifier & GLFW_MOD_ALT) result = result | (Input::Modifiers) Input::Modifier::ALT;
			if (modifier & GLFW_MOD_SUPER) result = result | (Input::Modifiers) Input::Modifier::META;
			return result;
		};


		Events::Key event
			{
				.keyCode = Input::IntoKeyCode(key).UnwrapOr(Input::KeyCode::Unknown),
				.scanCode = scancode,
				.modifiers = GetModifier(mods),
				.action = GetAction(action),
			};

		window->mEventQueue.emplace_back(event);
	}


	void Window::OnTextEvent(GLFWwindow* windowHandle, unsigned int codepoint)
	{
		Window* window = sInstanceMap.Lock()->at(windowHandle);

		Events::Text event
			{.codepoint = codepoint};

		window->mEventQueue.emplace_back(event);
	}


	void PollInput()
	{
		glfwPollEvents();
	}
}
