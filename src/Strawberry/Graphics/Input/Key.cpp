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
				{GLFW_KEY_A,             KeyCode::A},
				{GLFW_KEY_B,             KeyCode::B},
				{GLFW_KEY_C,             KeyCode::C},
				{GLFW_KEY_D,             KeyCode::D},
				{GLFW_KEY_E,             KeyCode::E},
				{GLFW_KEY_F,             KeyCode::F},
				{GLFW_KEY_G,             KeyCode::G},
				{GLFW_KEY_H,             KeyCode::H},
				{GLFW_KEY_I,             KeyCode::I},
				{GLFW_KEY_J,             KeyCode::J},
				{GLFW_KEY_K,             KeyCode::K},
				{GLFW_KEY_L,             KeyCode::L},
				{GLFW_KEY_L,             KeyCode::L},
				{GLFW_KEY_M,             KeyCode::M},
				{GLFW_KEY_N,             KeyCode::N},
				{GLFW_KEY_N,             KeyCode::N},
				{GLFW_KEY_O,             KeyCode::O},
				{GLFW_KEY_P,             KeyCode::P},
				{GLFW_KEY_Q,             KeyCode::Q},
				{GLFW_KEY_R,             KeyCode::R},
				{GLFW_KEY_S,             KeyCode::S},
				{GLFW_KEY_T,             KeyCode::T},
				{GLFW_KEY_U,             KeyCode::U},
				{GLFW_KEY_V,             KeyCode::V},
				{GLFW_KEY_W,             KeyCode::W},
				{GLFW_KEY_X,             KeyCode::X},
				{GLFW_KEY_Y,             KeyCode::Y},
				{GLFW_KEY_Z,             KeyCode::Z},
				{GLFW_KEY_0,             KeyCode::NUM_0},
				{GLFW_KEY_1,             KeyCode::NUM_1},
				{GLFW_KEY_2,             KeyCode::NUM_2},
				{GLFW_KEY_3,             KeyCode::NUM_3},
				{GLFW_KEY_4,             KeyCode::NUM_4},
				{GLFW_KEY_5,             KeyCode::NUM_5},
				{GLFW_KEY_6,             KeyCode::NUM_6},
				{GLFW_KEY_7,             KeyCode::NUM_7},
				{GLFW_KEY_8,             KeyCode::NUM_8},
				{GLFW_KEY_9,             KeyCode::NUM_9},
				{GLFW_KEY_LEFT_CONTROL,  KeyCode::RCTRL},
				{GLFW_KEY_RIGHT_CONTROL, KeyCode::RCTRL},
				{GLFW_KEY_LEFT_SHIFT,    KeyCode::LSHIFT},
				{GLFW_KEY_RIGHT_SHIFT,   KeyCode::RSHIFT},
				{GLFW_KEY_LEFT_ALT,      KeyCode::LALT},
				{GLFW_KEY_RIGHT_ALT,     KeyCode::RALT},
				{GLFW_KEY_LEFT_SUPER,    KeyCode::LMETA},
				{GLFW_KEY_RIGHT_SUPER,   KeyCode::RMETA},
				{GLFW_KEY_LEFT,          KeyCode::LEFT},
				{GLFW_KEY_RIGHT,         KeyCode::RIGHT},
				{GLFW_KEY_UP,            KeyCode::UP},
				{GLFW_KEY_DOWN,          KeyCode::DOWN},
				{GLFW_KEY_COMMA,         KeyCode::COMMA},
				{GLFW_KEY_PERIOD,        KeyCode::PERIOD},
				{GLFW_KEY_SEMICOLON,     KeyCode::SEMICOLON},
				{GLFW_KEY_APOSTROPHE,    KeyCode::QUOTE},
				{GLFW_KEY_SLASH,         KeyCode::FSLASH},
				{GLFW_KEY_BACKSLASH,     KeyCode::BSLASH},
				{GLFW_KEY_LEFT_BRACKET,  KeyCode::LBRACE},
				{GLFW_KEY_RIGHT_BRACKET, KeyCode::RBRACE},
				{GLFW_KEY_MINUS,         KeyCode::MINUS},
				{GLFW_KEY_EQUAL,         KeyCode::EQUALS},
				{GLFW_KEY_BACKSPACE,     KeyCode::BACKSPACE},
				{GLFW_KEY_ENTER,         KeyCode::ENTER},
				{GLFW_KEY_TAB,           KeyCode::TAB},
				{GLFW_KEY_CAPS_LOCK,     KeyCode::CAPSLOCK},
				{GLFW_KEY_F1,            KeyCode::F1},
				{GLFW_KEY_F2,            KeyCode::F2},
				{GLFW_KEY_F3,            KeyCode::F3},
				{GLFW_KEY_F4,            KeyCode::F4},
				{GLFW_KEY_F5,            KeyCode::F5},
				{GLFW_KEY_F6,            KeyCode::F6},
				{GLFW_KEY_F7,            KeyCode::F7},
				{GLFW_KEY_F8,            KeyCode::F8},
				{GLFW_KEY_F9,            KeyCode::F9},
				{GLFW_KEY_F10,           KeyCode::F10},
				{GLFW_KEY_F11,           KeyCode::F11},
				{GLFW_KEY_F12,           KeyCode::F12},
				{GLFW_KEY_F13,           KeyCode::F13},
				{GLFW_KEY_F14,           KeyCode::F14},
				{GLFW_KEY_F15,           KeyCode::F15},
				{GLFW_KEY_F16,           KeyCode::F16},
				{GLFW_KEY_F17,           KeyCode::F17},
				{GLFW_KEY_F18,           KeyCode::F18},
				{GLFW_KEY_F19,           KeyCode::F19},
				{GLFW_KEY_F20,           KeyCode::F20},
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
