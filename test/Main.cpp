#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Math/Matrix.hpp"
#include "Strawberry/Core/Timing/Clock.hpp"
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Vulkan/Memory/Allocator.hpp"
#include "Strawberry/Vulkan/Buffer.hpp"
#include "Strawberry/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Vulkan/CommandPool.hpp"
#include "Strawberry/Vulkan/ComputePipeline.hpp"
#include "Strawberry/Vulkan/Device.hpp"
#include "Strawberry/Vulkan/Framebuffer.hpp"
#include "Strawberry/Vulkan/GraphicsPipeline.hpp"
#include "Strawberry/Vulkan/Image.hpp"
#include "Strawberry/Vulkan/Instance.hpp"
#include "Strawberry/Vulkan/Queue.hpp"
#include "Strawberry/Vulkan/RenderPass.hpp"
#include "Strawberry/Vulkan/Sampler.hpp"
#include "Strawberry/Vulkan/Shader.hpp"
#include "Strawberry/Vulkan/Surface.hpp"
#include "Strawberry/Vulkan/Swapchain.hpp"
#include "Strawberry/Vulkan/Memory/FreelistAllocator.hpp"
#include "Strawberry/Window/Window.hpp"


using namespace Strawberry;
using namespace Vulkan;


void BasicRendering()
{
	uint8_t meshVertexShader[] =
	{
#include "Mesh.vert.bin"
	};

	uint8_t solidColorFragShader[] =
	{
#include "SolidColor.frag.bin"
	};

	uint8_t textureFragShader[] =
	{
#include "Texture.frag.bin"
	};


	uint8_t computeShaderCode[] =
{
#include "Pattern.comp.bin"
};


	Window::Window window("StrawberryGraphics Test", Core::Math::Vec2i(1920, 1080));
	Instance instance;
	const PhysicalDevice& gpu = instance.GetPhysicalDevices()[0];
	uint32_t queueFamily = gpu.SearchQueueFamilies(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)[0];
	uint32_t computeQueueFamily = gpu.SearchQueueFamilies(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)[0];


	Device device(gpu, {}, {QueueCreateInfo{queueFamily, 1}});
	Surface surface(window, device);
	RenderPass renderPass = RenderPass::Builder(device)
							.WithColorAttachment(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
												 VK_IMAGE_USAGE_TRANSFER_DST_BIT,
												 VK_FORMAT_R32G32B32A32_SFLOAT,
												 VK_ATTACHMENT_LOAD_OP_CLEAR,
												 VK_ATTACHMENT_STORE_OP_STORE,
												 VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
							.WithSubpass(SubpassDescription().WithColorAttachment(0, VK_IMAGE_LAYOUT_GENERAL))
							.Build();
	auto vertexShader = Shader::Compile(device, meshVertexShader)
		.Unwrap();
	auto fragmentShader = Shader::Compile(
		device, textureFragShader).Unwrap();
	PipelineLayout layout = PipelineLayout::Builder(device)
							.WithPushConstantRange(16 * sizeof(float), 0, VK_SHADER_STAGE_VERTEX_BIT)
							.WithPushConstantRange(3 * sizeof(float), 16 * sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT)
							.WithDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
							.Build();
	GraphicsPipeline pipeline = GraphicsPipeline::Builder(layout, renderPass, 0)
								.WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, std::move(vertexShader))
								.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader))
								.WithInputBinding(0, 3 * sizeof(float))
								.WithInputAttribute(0, 0, 0, VK_FORMAT_R32G32B32_SFLOAT)
								.WithInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
								.WithViewport({
												  VkViewport{
													  .x = 0, .y = 0, .width = 1920.0, .height = 1080.0,
													  .minDepth = 0.0, .maxDepth = 1.0
												  }
											  },
											  {VkRect2D{.offset = {0, 0}, .extent = {1920, 1080}}})
								.WithRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
												   VK_FRONT_FACE_COUNTER_CLOCKWISE)
								.WithColorBlending({
									VkPipelineColorBlendAttachmentState{
										.blendEnable = VK_TRUE,
										.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
										.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
										.colorBlendOp = VK_BLEND_OP_ADD,
										.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
										.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
										.alphaBlendOp = VK_BLEND_OP_ADD,
										.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
										VK_COLOR_COMPONENT_B_BIT |
										VK_COLOR_COMPONENT_A_BIT
									}
								})
								.WithMultisample(VK_SAMPLE_COUNT_1_BIT)
								.Build();
	auto queue = device.GetQueue(queueFamily, 0).GetReflexivePointer();
	auto computeQueue = device.GetQueue(computeQueueFamily, 0).GetReflexivePointer();
	Swapchain swapchain(*queue, surface, Core::Math::Vec2i(1920, 1080),
														   VK_PRESENT_MODE_IMMEDIATE_KHR);
	CommandPool commandPool(*queue, true);
	CommandBuffer commandBuffer(commandPool);

	auto hostVisibleMemoryType = gpu.SearchMemoryTypes(MemoryTypeCriteria::HostVisible())[0].index;
	auto deviceLocalMemoryType = gpu.SearchMemoryTypes(MemoryTypeCriteria::DeviceLocal())[0].index;

	FreeListAllocator hostVisibleAllocator(
		MemoryPool::Allocate(device, hostVisibleMemoryType, 128 * 1024 * 1024).Unwrap());
	FreeListAllocator deviceLocalAllocator(
		MemoryPool::Allocate(device, deviceLocalMemoryType, 128 * 1024 * 1024).Unwrap());


	Buffer buffer(hostVisibleAllocator,
				  6 * sizeof(float) * 3,
				  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	Core::IO::DynamicByteBuffer vertices;
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.0f, 0.0f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(1.0f, 0.0f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.0f, 1.0f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.5f, 0.5f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(1.0f, 0.5f, 0.0f));
	vertices.Push<Core::Math::Vec3f>(Core::Math::Vec3f(0.5f, 1.0f, 0.0f));
	buffer.SetData(vertices);


	auto framebuffer = Framebuffer(renderPass, deviceLocalAllocator, Core::Math::Vec2u(1920, 1080));


	auto [size, channels, bytes] = Core::IO::DynamicByteBuffer::FromImage("data/dio.png").Unwrap();
	Buffer textureBuffer(hostVisibleAllocator, bytes.Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	textureBuffer.SetData(bytes);
	Image texture = Image::Builder(deviceLocalAllocator)
					.WithExtent(size).WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
					.WithUsage(
						VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT).
					Build();

	commandBuffer.Begin(true);
	commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
								  VK_PIPELINE_STAGE_TRANSFER_BIT,
								  0,
								  {
									  ImageMemoryBarrier(texture, VK_IMAGE_ASPECT_COLOR_BIT).ToLayout(
										  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
								  });
	commandBuffer.CopyBufferToImage(textureBuffer, texture);
	commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
								  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
								  0,
								  {
									  ImageMemoryBarrier(texture, VK_IMAGE_ASPECT_COLOR_BIT).FromLayout(
										  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL).ToLayout(
										  VK_IMAGE_LAYOUT_GENERAL)
								  });
	commandBuffer.End();
	queue->Submit(commandBuffer);
	queue->WaitUntilIdle();
	ImageView textureView = texture.Create<ImageView::Builder>(VK_IMAGE_ASPECT_COLOR_BIT)
								   .WithType(VK_IMAGE_VIEW_TYPE_2D)
								   .WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
								   .Build();
	Sampler sampler(device, VK_FILTER_NEAREST, VK_FILTER_NEAREST);


	Core::Clock clock;
	DescriptorPool descriptorPool(device, 0, 1, {
											  VkDescriptorPoolSize{
												  .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
												  .descriptorCount = 1
											  }
										  });
	DescriptorSet textureDescriptorSet(descriptorPool, layout.GetSetLayout(0));


	Shader computeShader = Shader::Compile(device, computeShaderCode).Unwrap();


	PipelineLayout computePipelineLayout = PipelineLayout::Builder(device)
		.WithDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
		.Build();

	ComputePipeline computePipeline = ComputePipeline::Builder(device, computePipelineLayout, std::move(computeShader))
		.Build();


	DescriptorPool computeDescriptorPool(device, 0, 1, { VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1}});
	DescriptorSet computeDescriptorSet(computeDescriptorPool, computePipelineLayout.GetSetLayout(0));
	Buffer computeBuffer(hostVisibleAllocator, sizeof(uint32_t) * 256 * 256, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	computeDescriptorSet.SetStorageBuffer(0, 0, computeBuffer);


	while (!window.CloseRequested())
	{



		CommandBuffer computeCommandBuffer(commandPool);
		computeCommandBuffer.Begin();
		computeCommandBuffer.BindPipeline(computePipeline);
		computeCommandBuffer.BindDescriptorSet(computePipeline, 0, computeDescriptorSet);
		computeCommandBuffer.Dispatch(256 * 256);
		computeCommandBuffer.End();
		computeQueue->Submit(std::move(computeCommandBuffer));
		computeQueue->WaitUntilIdle();

		auto computeData = computeBuffer.GetData();
		uint32_t* computeDataAsInts = reinterpret_cast<::uint32_t*>(computeData);
		for (int i = 0; i < 256 * 256; i++)
		{
			bool off = i % 2 == 0;
			Core::AssertEQ(computeDataAsInts[i], off ? 255 : 0);
		}


		Image& renderTarget = *swapchain.WaitForNextImage().Unwrap();
		Window::PollInput();

		while (auto event = window.NextEvent())
		{
			if (auto text = event->Ptr<Window::Events::Text>())
			{
				std::string c = Core::ToUTF8(text->codepoint).Unwrap();
				std::cout << (const char*)c.data() << std::endl;
			}
		}


		Core::Math::Mat4f MVPMatrix;
		Core::Math::Vec3f Color((std::sin(*clock) + 1.0f) / 2.0f,
								(std::cos(0.25 * *clock) + 1.0f) / 2.0f,
								(std::cos(0.5 * *clock) + 1.0f) / 2.0f);


		textureDescriptorSet.SetUniformTexture(0, 0, sampler, textureView, VK_IMAGE_LAYOUT_GENERAL);


		commandBuffer.Begin(true);
		commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									  0,
									  {
										  ImageMemoryBarrier(framebuffer.GetAttachment(0), VK_IMAGE_ASPECT_COLOR_BIT).
										  ToLayout(VK_IMAGE_LAYOUT_GENERAL)
									  });
		commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									  0,
									  {
										  ImageMemoryBarrier(framebuffer.GetAttachment(0), VK_IMAGE_ASPECT_COLOR_BIT).
										  ToLayout(VK_IMAGE_LAYOUT_GENERAL)
									  });
		commandBuffer.BeginRenderPass(renderPass, framebuffer);
		commandBuffer.BindPipeline(pipeline);
		commandBuffer.BindVertexBuffer(0, buffer);
		commandBuffer.BindDescriptorSet(pipeline, 0, textureDescriptorSet);
		commandBuffer.PushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, Core::IO::DynamicByteBuffer(MVPMatrix), 0);
		commandBuffer.PushConstants(pipeline, VK_SHADER_STAGE_FRAGMENT_BIT, Core::IO::DynamicByteBuffer(Color), 64);
		commandBuffer.Draw(6);
		commandBuffer.EndRenderPass();
		commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
									  0,
									  {
										  ImageMemoryBarrier(renderTarget, VK_IMAGE_ASPECT_COLOR_BIT).ToLayout(
											  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
									  });
		commandBuffer.BlitImage(framebuffer.GetAttachment(0),
								VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
								renderTarget,
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								VK_IMAGE_ASPECT_COLOR_BIT,
								VK_FILTER_NEAREST);
		commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
									  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
									  0,
									  {
										  ImageMemoryBarrier(renderTarget, VK_IMAGE_ASPECT_COLOR_BIT).FromLayout(
											  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL).ToLayout(
											  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
									  });
		commandBuffer.End();
		queue->Submit(commandBuffer);
		queue->WaitUntilIdle();


		swapchain.Present();
		window.SwapBuffers();
	}
}


int main()
{
	BasicRendering();
	return 0;
}
