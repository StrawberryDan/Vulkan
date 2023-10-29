#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Units.hpp"


namespace Strawberry::Graphics
{
	class Transform2D
	{
	private:
		Core::Math::Vec2f mPosition;
		Core::Math::Vec2f mScale = Core::Math::Vec2f(1.0f, 1.0f);
		Core::Math::Radians mOrientation = 0.0f;
	};
}
