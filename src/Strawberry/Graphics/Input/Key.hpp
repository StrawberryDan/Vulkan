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
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		NUM_5,
		NUM_6,
		NUM_7,
		NUM_8,
		NUM_9,
	};


	Core::Optional<KeyCode> IntoKeyCode(int glfwKeyCode);


	using Modifiers = uint8_t;


	enum class Modifier
		: Modifiers
	{
		CTRL = 1 << 0,
		SHIFT = 1 << 1,
		ALT = 1 << 2,
		META = 1 << 3,
	};


	using ScanCode = int;


	enum class KeyAction
	{
		Press, Release, Repeat,
	};
}