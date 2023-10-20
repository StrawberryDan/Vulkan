
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
#include "Strawberry/Graphics/ShaderModule.hpp"


int main()
{
	using namespace Strawberry;
	using namespace Graphics;

	auto vertexInputDescription = [] () -> VertexInputDescription
	{
		VertexInputDescription description;
		description.AddBinding(3 * 4)
			.WithAttribute(0, VK_FORMAT_R32G32B32_SFLOAT, 0);
		return description;
	};

	Window::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));
	Instance instance;
	Device device = instance.Create<Device>();
	Surface surface = window.Create<Surface, const Device&>(device);
	Pipeline pipeline = device.Create<Pipeline::Builder>()
		.WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, device.Create<ShaderModule>("data/Shaders/Mesh.vert.spirv"))
		.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, device.Create<ShaderModule>("data/Shaders/SolidColor.frag.spirv"))
	    .WithVertexInput(vertexInputDescription())
		.WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.WithViewportSize(Core::Math::Vec2i(1920, 1080))
		.Build();
	Swapchain swapchain = device.Create<Swapchain, const Surface&>(surface, Core::Math::Vec2i{1920, 1080});
	Queue queue = device.Create<Queue>();
	CommandPool commandPool = device.Create<CommandPool>();
	CommandBuffer commandBuffer = commandPool.Create<CommandBuffer>();
	Buffer buffer = device.Create<Buffer>(1024, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	Image image = device.Create<Image>(Core::Math::Vec2i(100, 100), VK_FORMAT_R8G8B8A8_UINT, VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	commandBuffer.Begin();
	commandBuffer.End();

	while (!window.CloseRequested())
	{
		window.SwapBuffers();
		Window::PollInput();

		while (auto event = window.NextEvent())
		{
			if (auto text = event->Value<Window::Events::Text>())
			{
				std::u8string c = Core::ToUTF8(text->codepoint).Unwrap();
				std::cout << (const char*) c.data() << std::endl;
			}
		}
	}

	return 0;
}
