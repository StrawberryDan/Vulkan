//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Key.hpp"
// GLFW
#include "GLFW/glfw3.h"
// Standard Library
#include <map>


namespace Strawberry::Graphics::Input
{
	Core::Optional<KeyCode> IntoKeyCode(int glfwKeyCode)
	{
		static const std::map<int, KeyCode> sMapping
			{
				{GLFW_KEY_A, KeyCode::A}, {GLFW_KEY_B, KeyCode::B}, {GLFW_KEY_C, KeyCode::C},
				{GLFW_KEY_D, KeyCode::D}, {GLFW_KEY_E, KeyCode::E}, {GLFW_KEY_F, KeyCode::F},
				{GLFW_KEY_G, KeyCode::G}, {GLFW_KEY_H, KeyCode::H}, {GLFW_KEY_I, KeyCode::I},
				{GLFW_KEY_J, KeyCode::J}, {GLFW_KEY_K, KeyCode::K}, {GLFW_KEY_L, KeyCode::L},
				{GLFW_KEY_L, KeyCode::L}, {GLFW_KEY_M, KeyCode::M}, {GLFW_KEY_N, KeyCode::N},
				{GLFW_KEY_N, KeyCode::N}, {GLFW_KEY_O, KeyCode::O}, {GLFW_KEY_P, KeyCode::P},
				{GLFW_KEY_Q, KeyCode::Q}, {GLFW_KEY_R, KeyCode::R}, {GLFW_KEY_S, KeyCode::S},
				{GLFW_KEY_T, KeyCode::T}, {GLFW_KEY_U, KeyCode::U}, {GLFW_KEY_V, KeyCode::V},
				{GLFW_KEY_W, KeyCode::W}, {GLFW_KEY_X, KeyCode::X}, {GLFW_KEY_Y, KeyCode::Y},
				{GLFW_KEY_Z, KeyCode::Z},
			};

		if (sMapping.contains(glfwKeyCode))
		{
			return sMapping.at(glfwKeyCode);
		}
		else
		{
			return Core::NullOpt;
		}
	}
}