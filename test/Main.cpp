
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Graphics/Instance.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Strawberry/Graphics/Device.hpp"


int main()
{
	using namespace Strawberry;

	Graphics::Instance instance;
	Graphics::Device device = instance.Create<Graphics::Device>();

	Graphics::Window::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));

	while (!window.CloseRequested())
	{
		Graphics::Window::PollInput();

		while (auto event = window.NextEvent())
		{
			if (auto text = event->Value<Graphics::Window::Events::Text>())
			{
				std::u8string c = Core::ToUTF8(text->codepoint).Unwrap();
				std::cout << (const char*) c.data() << std::endl;
			}
		}
	}

	return 0;
}
