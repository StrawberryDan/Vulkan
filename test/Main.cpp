
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
#include "Strawberry/Graphics/ShaderModule.hpp"
#include "Strawberry/Graphics/Framebuffer.hpp"
#include "Strawberry/Core/Math/Matrix.hpp"


int main()
{
	using namespace Strawberry;
	using namespace Graphics;

	auto vertexInputDescription = []() -> VertexInputDescription
	{
		VertexInputDescription description;
		description.AddBinding(3 * sizeof(float))
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
		.WithPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 16 * sizeof(float), 0)
		.WithPushConstantRange(VK_SHADER_STAGE_FRAGMENT_BIT, 3 * sizeof(float), 16 * sizeof(float))
		.Build();
	Swapchain swapchain = device.Create<Swapchain, const Surface&>(surface, Core::Math::Vec2i(1920, 1080));
	Queue queue = device.Create<Queue>();
	CommandPool commandPool = device.Create<CommandPool>(true);
	CommandBuffer commandBuffer = commandPool.Create<CommandBuffer>();


	Buffer buffer = device.Create<Buffer>(6 * sizeof(float) * 3, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	Core::IO::DynamicByteBuffer vertices;
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.0f, 0.0f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(1.0f, 0.0f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.0f, 1.0f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.5f, 0.5f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(1.0f, 0.5f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.5f, 1.0f, 0.0f));
	buffer.SetData(vertices);

	Framebuffer framebuffer = pipeline.Create<Framebuffer>();


	while (!window.CloseRequested())
	{
		Window::PollInput();

		while (auto event = window.NextEvent())
		{
			if (auto text = event->Value<Window::Events::Text>())
			{
				std::u8string c = Core::ToUTF8(text->codepoint).Unwrap();
				std::cout << (const char*) c.data() << std::endl;
			}
		}


		Core::Math::Mat4f MVPMatrix;
		Core::Math::Vec3f Color(1.0f, 0.5f, 0.5f);


		commandBuffer.Begin(false);
		commandBuffer.BeginRenderPass(pipeline, framebuffer);
		commandBuffer.BindPipeline(pipeline);
		commandBuffer.BindVertexBuffer(0, buffer);
		commandBuffer.PushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, Core::IO::DynamicByteBuffer(MVPMatrix), 0);
		commandBuffer.PushConstants(pipeline, VK_SHADER_STAGE_FRAGMENT_BIT, Core::IO::DynamicByteBuffer(Color), 64);
		commandBuffer.Draw(6);
		commandBuffer.EndRenderPass();
		commandBuffer.CopyToSwapchain(swapchain, framebuffer.GetColorAttachment(0));
		commandBuffer.ImageMemoryBarrier(swapchain.GetNextImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		commandBuffer.End();
		queue.Submit(commandBuffer);


		swapchain.Present(queue);
		window.SwapBuffers();
	}

	return 0;
}
