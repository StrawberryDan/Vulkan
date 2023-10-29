//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Sprite.hpp"
#include "SpriteSheet.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Sprite::Sprite(const SpriteSheet& spriteSheet)
		: mSpriteSheet(spriteSheet)
	{}


	Transform2D& Sprite::GetTransform()
	{
		return mTransform;
	}


	const Transform2D& Sprite::GetTransform() const
	{
		return mTransform;
	}


	void Sprite::SetSpriteSheet(const SpriteSheet& spriteSheet)
	{
		mSpriteSheet = spriteSheet.GetReflexivePointer();
	}


	const Core::Math::Vec2u& Sprite::GetSpriteCoords() const
	{
		return mSpriteCoords;
	}


	void Sprite::SetSpriteCoords(const Core::Math::Vec2u& spriteCoords)
	{
		mSpriteCoords = spriteCoords;
	}


	const Core::ReflexivePointer<SpriteSheet>& Sprite::GetSpriteSheet() const
	{
		return mSpriteSheet;
	}
}
