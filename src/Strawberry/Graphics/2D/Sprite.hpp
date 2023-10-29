#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
#include "Transform2D.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"


namespace Strawberry::Graphics
{
	class SpriteSheet;


	class Sprite
	{
		friend class SpriteRenderer;


	public:
		Sprite(const SpriteSheet& spriteSheet);


		Transform2D& GetTransform();
		const Transform2D& GetTransform() const;
		void SetSpriteSheet(const SpriteSheet& spriteSheet);
		void SetSpriteSheetIndex(const uint32_t index);


	private:
		Core::ReflexivePointer<SpriteSheet> mSpriteSheet;

		Transform2D mTransform;
		uint32_t mSpriteSheetIndex = 0;
	};
} // Graphics
