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


	uint32_t Sprite::GetSpriteIndex() const
	{
		return mSpriteCoords[0] + mSpriteSheet->GetSpriteCount()[0] * mSpriteCoords[1];
	}


	void Sprite::SetSpriteIndex(uint32_t index)
	{
		index = index % (mSpriteSheet->GetSpriteCount()[0] * mSpriteSheet->GetSpriteCount()[1]);
		mSpriteCoords = {
			index % mSpriteSheet->GetSpriteCount()[0],
			index / mSpriteSheet->GetSpriteCount()[1]
		};
	}
}
