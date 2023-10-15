
#include "Strawberry/Graphics/Instance.hpp"
#include "Strawberry/Graphics/Window.hpp"


int main()
{
	using namespace Strawberry;

	Graphics::Instance instance;

	Graphics::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));

	return 0;
}