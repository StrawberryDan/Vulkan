
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Graphics/Vulkan/Instance.hpp"
#include "Strawberry/Graphics/Window.hpp"
#include "Strawberry/Graphics/Vulkan/Device.hpp"
#include "Strawberry/Graphics/Vulkan/Pipeline.hpp"
#include "Strawberry/Graphics/Vulkan/Swapchain.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
#include "Strawberry/Graphics/Vulkan/CommandPool.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Graphics/Vulkan/Buffer.hpp"
#include "Strawberry/Graphics/Vulkan/BufferView.hpp"
#include "Strawberry/Graphics/Vulkan/Image.hpp"
#include "Strawberry/Graphics/Vulkan/ShaderModule.hpp"
#include "Strawberry/Graphics/Vulkan/Framebuffer.hpp"
#include "Strawberry/Core/Math/Matrix.hpp"
#include "Strawberry/Core/Timing/Clock.hpp"
#include "Strawberry/Graphics/Vulkan/Sampler.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/2D/SpriteRenderer.hpp"
#include "Strawberry/Graphics/2D/Sprite.hpp"
#include "Strawberry/Graphics/2D/SpriteSheet.hpp"


using namespace Strawberry;
using namespace Graphics;
using namespace Vulkan;


void BasicRendering()
{
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
	RenderPass renderPass = device.Create<RenderPass::Builder>()
		.WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE)
		.WithSubpass(SubpassDescription().WithColorAttachment(0))
		.Build();
	Pipeline pipeline = renderPass.Create<Pipeline::Builder>()
		.WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, device.Create<ShaderModule>("data/Shaders/Mesh.vert.spirv"))
		.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
						 device.Create<ShaderModule>("data/Shaders/Texture.frag.spirv"))
		.WithVertexInput(vertexInputDescription())
		.WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.WithViewport({0, 0}, {1920, 1080})
		.WithPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 16 * sizeof(float), 0)
		.WithPushConstantRange(VK_SHADER_STAGE_FRAGMENT_BIT, 3 * sizeof(float), 16 * sizeof(float))
		.WithDescriptorSetLayout(
			DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT))
		.Build();
	Queue queue = device.Create<Queue>(	);
	Swapchain swapchain = queue.Create<Swapchain, const Surface&>(surface, Core::Math::Vec2i(1920, 1080));
	CommandPool commandPool = queue.Create<CommandPool>(true);
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


	Framebuffer framebuffer = renderPass.Create<Framebuffer>(Core::Math::Vec2i(1920, 1080));


	auto [size, channels, bytes] = Core::IO::DynamicByteBuffer::FromImage("data/dio.png").Unwrap();
	Buffer textureBuffer = device.Create<Buffer>(bytes.Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	textureBuffer.SetData(bytes);
	Image texture = device.Create<Image>(size, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	commandBuffer.Begin(true);
	commandBuffer.CopyBufferToImage(textureBuffer, texture, VK_FORMAT_R8G8B8A8_SRGB);
	commandBuffer.ImageMemoryBarrier(texture, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL);
	commandBuffer.End();
	queue.Submit(commandBuffer);
	ImageView textureView = texture.Create<ImageView::Builder>()
		.WithType(VK_IMAGE_VIEW_TYPE_2D)
		.WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
		.Build();
	Sampler sampler = device.Create<Sampler>(VK_FILTER_NEAREST, VK_FILTER_NEAREST);


	Core::Clock clock;


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
		Core::Math::Vec3f Color((std::sinf(*clock) + 1.0f) / 2.0f,
								(std::cosf(0.25 * *clock) + 1.0f) / 2.0f,
								(std::cosf(0.5 * *clock) + 1.0f) / 2.0f);


		pipeline.SetUniformTexture(sampler, textureView, VK_IMAGE_LAYOUT_GENERAL, 0, 0);


		commandBuffer.Begin(true);
		commandBuffer.BeginRenderPass(renderPass, framebuffer);
		commandBuffer.BindPipeline(pipeline);
		commandBuffer.BindVertexBuffer(0, buffer);
		commandBuffer.BindDescriptorSet(pipeline, 0);
		commandBuffer.PushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, Core::IO::DynamicByteBuffer(MVPMatrix), 0);
		commandBuffer.PushConstants(pipeline, VK_SHADER_STAGE_FRAGMENT_BIT, Core::IO::DynamicByteBuffer(Color), 64);
		commandBuffer.Draw(6);
		commandBuffer.EndRenderPass();
		commandBuffer.End();
		queue.Submit(commandBuffer);


		swapchain.Present(framebuffer);
		window.SwapBuffers();
	}
}


void SpriteRendering()
{
	Window::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));
	Instance instance;
	Device device = instance.Create<Device>();
	Surface surface = window.Create<Surface, const Device&>(device);
	RenderPass renderPass = device.Create<RenderPass::Builder>()
		.WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE)
		.WithSubpass(SubpassDescription().WithColorAttachment(0))
		.Build();
	Queue queue = device.Create<Queue>();
	Swapchain swapchain = queue.Create<Swapchain, const Surface&>(surface, window.GetSize());

	Framebuffer framebuffer = renderPass.Create<Framebuffer>(Core::Math::Vec2i(1920, 1080));

	SpriteRenderer renderer(queue, {1920, 1080});
	SpriteSheet spriteSheet = SpriteSheet::FromFile(device, queue, {500, 500}, "data/dio.png").Unwrap();
	Sprite sprite = spriteSheet.Create<Sprite>();
	sprite.SetSpriteCoords({1, 1});
	sprite.GetTransform().SetSize({1920 - 100, 1080 - 100});
	sprite.GetTransform().SetPosition({100, 100});

	while (!window.CloseRequested())
	{
		Window::PollInput();

		renderer.Draw(framebuffer, sprite);

		swapchain.Present(framebuffer);

		window.SwapBuffers();
	}
}


int main()
{
	BasicRendering();
	SpriteRendering();
	return 0;
}
