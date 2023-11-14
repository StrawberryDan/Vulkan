#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Graphics
#include "Strawberry/Graphics/Input/Key.hpp"
#include "Strawberry/Graphics/Input/Mouse.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Variant.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Timing/Clock.hpp"


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

		struct MouseMove
		{
			Core::Math::Vec2f position;
			Core::Math::Vec2f deltaPosition;
		};


		struct MouseButton
		{
			Input::MouseButton button;
			Input::Modifiers modifiers;
			Input::KeyAction action;
			Core::Math::Vec2f position;
		};
	}


	using Event = Core::Variant<Events::Key,
								Events::Text,
								Events::MouseMove,
								Events::MouseButton>;
}
