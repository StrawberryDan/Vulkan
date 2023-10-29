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
		[[nodiscard]] const Transform2D& GetTransform() const;
		[[nodiscard]] const Core::ReflexivePointer<SpriteSheet>& GetSpriteSheet() const;
		void SetSpriteSheet(const SpriteSheet& spriteSheet);
		[[nodiscard]] const Core::Math::Vec2u& GetSpriteCoords() const;
		void SetSpriteCoords(const Core::Math::Vec2u& spriteCoords);


	private:
		Core::ReflexivePointer<SpriteSheet> mSpriteSheet;

		Transform2D mTransform;
		Core::Math::Vec2u mSpriteCoords;
	};
} // Graphics
