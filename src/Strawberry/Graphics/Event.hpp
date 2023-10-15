#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
#include "Strawberry/Graphics/Input/Key.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Variant.hpp"


namespace Strawberry::Graphics::Window
{
	namespace Events
	{
		struct Key
		{
			Input::KeyCode keyCode;
			Input::ScanCode scanCode;
			Input::Modifiers modifiers;
			Input::KeyAction action;
		};

		struct Text
		{
			char32_t codepoint;
		};
	}


	using Event = Core::Variant<Events::Key, Events::Text>;
}
