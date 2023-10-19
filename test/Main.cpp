
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Graphics/Instance.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Strawberry/Graphics/Device.hpp"
#include "Strawberry/Graphics/Pipeline.hpp"
#include "Strawberry/Graphics/Swapchain.hpp"
#include "Strawberry/Graphics/Queue.hpp"
#include "Strawberry/Graphics/CommandPool.hpp"
#include "Strawberry/Graphics/CommandBuffer.hpp"
#include "Strawberry/Graphics/Buffer.hpp"
#include "Strawberry/Graphics/BufferView.hpp"
#include "Strawberry/Graphics/Image.hpp"
#include "Strawberry/Graphics/ImageView.hpp"


int main()
{
	using namespace Strawberry;

	Graphics::Window::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));
	Graphics::Instance instance;
	Graphics::Device device = instance.Create<Graphics::Device>();
	Graphics::Surface surface = window.Create<Graphics::Surface, const Graphics::Device&>(device);
	Graphics::Pipeline pipeline = device.Create<Graphics::Pipeline::Builder>().Build();
	Graphics::Swapchain swapchain = device.Create<Graphics::Swapchain, const Graphics::Surface&>(surface, Core::Math::Vec2i{1920, 1080});
	Graphics::Queue queue = device.Create<Graphics::Queue>();
	Graphics::CommandPool commandPool = device.Create<Graphics::CommandPool>();
	Graphics::CommandBuffer commandBuffer = commandPool.Create<Graphics::CommandBuffer>();
	Graphics::Buffer buffer = device.Create<Graphics::Buffer>(1024, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	Graphics::Image image = device.Create<Graphics::Image>(Core::Math::Vec2i(100, 100), VK_FORMAT_R8G8B8A8_UINT, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	Graphics::ImageView imageView = image.Create<Graphics::ImageView>(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UINT);

	commandBuffer.Begin();
	commandBuffer.End();

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
