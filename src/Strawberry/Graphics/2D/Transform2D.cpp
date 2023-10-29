//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Transform2D.hpp"
// Strawberry Core
#include <Strawberry/Core/Math/Transformations.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	const Core::Math::Vec2f& Transform2D::GetPosition() const
	{
		return mPosition;
	}


	const Core::Math::Vec2f& Transform2D::GetSize() const
	{
		return mSize;
	}


	void Transform2D::SetPosition(const Core::Math::Vec2f position)
	{
		mPosition = position;
	}


	void Transform2D::SetSize(const Core::Math::Vec2f& size)
	{
		mSize = size;
	}


	Core::Math::Mat4f Transform2D::AsMatrix() const
	{
		return Core::Math::Translate(mPosition.WithAdditionalValues(0.0)) * Core::Math::Scale(mSize.WithAdditionalValues(1.0));
	}
}
