#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
// GLFW 3
#include "GLFW/glfw3.h"
// Standard Library
#include <string>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Window
	{
	private:
		static void Initialise();
		static void Terminate();


		static std::atomic<unsigned int> sInstanceCount;


	public:
		Window(const std::string& title, Core::Math::Vec2i size);
		Window(const Window& rhs) = default;
		Window& operator=(const Window& rhs) = default;
		Window(Window&& rhs);
		Window& operator=(Window&& rhs);
		~Window();

	private:
		GLFWwindow* mHandle;
	};
}
