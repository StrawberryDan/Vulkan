
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Graphics/Instance.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Strawberry/Graphics/Device.hpp"
#include "Strawberry/Graphics/Pipeline.hpp"


int main()
{
	using namespace Strawberry;

	Graphics::Window::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));
	Graphics::Instance instance;
	Graphics::Surface surface = window.CreateSurface(instance);
	Graphics::Device device = instance.Create<Graphics::Device>();
	Graphics::Pipeline pipeline = device.Create<Graphics::Pipeline::Builder>().Build();

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
