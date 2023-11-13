#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Vulkan/Image.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// FreeType
#include "ft2build.h"
#include FT_FREETYPE_H
// Standard Library
#include <filesystem>



//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	namespace FreeType
	{
		void Initialise();
		void Terminate();
	}


	class FontFace
		: public Core::EnableReflexivePointer<FontFace>
	{
	public:
		static Core::Optional<FontFace> FromFile(const std::filesystem::path& file);


		FontFace(const FontFace& rhs) = default;
		FontFace& operator=(const FontFace& rhs) = default;
		FontFace(FontFace&& rhs) noexcept;
		FontFace& operator=(FontFace&& rhs) noexcept;
		~FontFace();


		Core::Math::Vec2f GetGlyphBoundingBox(char32_t c) const;
		Core::Math::Vec2f GetGlyphHorizontalBearing(char32_t c) const;
		Core::Math::Vec2f GetGlyphAdvance(char32_t c) const;
		Core::Optional<Vulkan::Image> GetGlyphBitmap(Vulkan::Queue& queue, char32_t c) const;


		void SetPixelSize(uint32_t pixelSize);
		void SetPixelSize(Core::Math::Vec2u pixelSize);


	protected:
		FontFace() = default;


	private:
		FT_Face mFace;
	};
}
