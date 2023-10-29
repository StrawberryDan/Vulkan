#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Math/Matrix.hpp"


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
	public:
		void SetSize(const Core::Math::Vec2f& size);


		[[nodiscard]] Core::Math::Mat4f AsMatrix() const;


	private:
		Core::Math::Vec2f   mPosition;
		Core::Math::Vec2f   mSize        = Core::Math::Vec2f(1.0f, 1.0f);
		Core::Math::Radians mOrientation = 0.0f;
	};
}
