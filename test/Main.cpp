
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
#include "Strawberry/Graphics/Framebuffer.hpp"


int main()
{
	using namespace Strawberry;
	using namespace Graphics;

	auto vertexInputDescription = []() -> VertexInputDescription
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
		.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
						 device.Create<ShaderModule>("data/Shaders/SolidColor.frag.spirv"))
		.WithVertexInput(vertexInputDescription())
		.WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.WithViewportSize(Core::Math::Vec2i(1920, 1080))
		.Build();
	Swapchain swapchain = device.Create<Swapchain, const Surface&>(surface, Core::Math::Vec2i{1920, 1080});
	Queue queue = device.Create<Queue>();
	CommandPool commandPool = device.Create<CommandPool>(true);
	CommandBuffer commandBuffer = commandPool.Create<CommandBuffer>();
	Image image = device.Create<Image>(Core::Math::Vec2i(1920, 1080), VK_FORMAT_R32G32B32A32_SFLOAT,
									   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	ImageView imageView = image.Create<ImageView>(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT);


	Buffer buffer = device.Create<Buffer>(3 * sizeof(float) * 3, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	Core::IO::DynamicByteBuffer verticies;
	verticies.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.0f, 0.0f, 0.0f));
	verticies.Push<Core::Math::Vec3f>(Core::Math::Vec3f(1.0f, 0.0f, 0.0f));
	verticies.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.0f, 1.0f, 0.0f));
	buffer.SetData(verticies);

	Framebuffer framebuffer = pipeline.Create<Framebuffer, const ImageView&>(imageView);


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


		commandBuffer.Begin(false);
		commandBuffer.ImageMemoryBarrier(image, VK_IMAGE_LAYOUT_GENERAL);
		commandBuffer.BeginRenderPass(pipeline, framebuffer);
		commandBuffer.BindPipeline(pipeline);
		commandBuffer.BindVertexBuffer(0, buffer);
		commandBuffer.Draw(3);
		commandBuffer.EndRenderPass();
		commandBuffer.End();
		queue.Submit(commandBuffer);
	}

	return 0;
}
