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


	void Sprite::SetSpriteSheetIndex(const uint32_t index)
	{
		mSpriteSheetIndex = index;
	}
}
