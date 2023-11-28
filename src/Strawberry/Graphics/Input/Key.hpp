#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
// GLFW
#include "GLFW/glfw3.h"
// Standard Library
#include <cstdint>


namespace Strawberry::Graphics::Input
{
	enum class KeyCode
	{
		Unknown,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		NUM_0,
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		NUM_5,
		NUM_6,
		NUM_7,
		NUM_8,
		NUM_9,
		LCTRL,
		RCTRL,
		LSHIFT,
		RSHIFT,
		LALT,
		RALT,
		LMETA,
		RMETA,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		COMMA,
		PERIOD,
		SEMICOLON,
		QUOTE,
		FSLASH,
		BSLASH,
		LBRACE,
		RBRACE,
		MINUS,
		EQUALS,
		BACKSPACE,
		ENTER,
		TAB,
		CAPSLOCK,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
	};


	Core::Optional<KeyCode> IntoKeyCode(int glfwKeyCode);


	using Modifiers = uint8_t;


	class Modifier
	{
	public:
		enum : Modifiers
		{
			CTRL = 1 << 0,
			SHIFT = 1 << 1,
			ALT = 1 << 2,
			META = 1 << 3,
		};
	};


	using ScanCode = int;


	enum class KeyAction
	{
		Press, Release, Repeat,
	};
}
