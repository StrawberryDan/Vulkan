//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ImageRenderer.hpp"
#include "Strawberry/Core/Math/Matrix.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Core/Math/Transformations.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	ImageRenderer::ImageRenderer(Vulkan::Queue& queue, Core::Math::Vec2u resolution)
		: Renderer(queue, CreateRenderPass(*queue.GetDevice()), resolution)
		, mQueue(queue)
		, mPipeline(CreatePipeline())
		, mDescriptorSet(mPipeline.AllocateDescriptorSet(0))
		, mVertexUniformBuffer(*queue.GetDevice(), 2 * sizeof(Core::Math::Mat4f), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
		, mTextureSampler(*queue.GetDevice(), mMagFilter, mMinFilter)
	{}


	void ImageRenderer::Draw(Vulkan::Image& image, Core::Math::Vec2f position)
	{
		Draw(image, position, image.GetSize().AsSize<2>().AsType<float>());
	}


	void ImageRenderer::Draw(Vulkan::Image& image, Core::Math::Vec2f position, Core::Math::Vec2f size)
	{
		auto commandBuffer = mQueue->Create<Vulkan::CommandBuffer>();


		Core::IO::DynamicByteBuffer vertexUniformBytes;
		Core::Math::Mat4f viewMatrix = Core::Math::Translate(Core::Math::Vec3f(-1.0, -1.0, 0.0)) * Core::Math::Scale(GetResolution().AsType<float>().Map([](auto x) {return 2.0f / x;}).WithAdditionalValues(1.0f));
		Core::Math::Mat4f modelMatrix = Core::Math::Translate(position.WithAdditionalValues(0.0f)) * Core::Math::Scale(size.WithAdditionalValues(1.0f));
		vertexUniformBytes.Push(viewMatrix);
		vertexUniformBytes.Push(modelMatrix);
		mVertexUniformBuffer.SetData(vertexUniformBytes);
		mDescriptorSet.SetUniformBuffer(mVertexUniformBuffer, 0);


		Vulkan::ImageView view = Vulkan::ImageView::Builder(image)
			.WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
			.WithType(VK_IMAGE_VIEW_TYPE_2D)
			.Build();
		mDescriptorSet.SetUniformTexture(mTextureSampler, view, VK_IMAGE_LAYOUT_GENERAL, 1);



		commandBuffer.Begin(true);
		commandBuffer.BindPipeline(mPipeline);
		commandBuffer.BindDescriptorSet(mPipeline, 0, mDescriptorSet);
		commandBuffer.ImageMemoryBarrier(image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL);
		commandBuffer.BeginRenderPass(*GetRenderPass(), GetFramebuffer());
		commandBuffer.Draw(4);
		commandBuffer.EndRenderPass();
		commandBuffer.End();
		mQueue->Submit(std::move(commandBuffer));
		mQueue->Wait();
	}


	Vulkan::RenderPass ImageRenderer::CreateRenderPass(Vulkan::Device& device)
	{
		return Vulkan::RenderPass::Builder(device)
			.WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE)
			.WithSubpass(Vulkan::SubpassDescription().WithColorAttachment(0))
			.Build();
	}


	Vulkan::Pipeline ImageRenderer::CreatePipeline()
	{
		uint8_t vertexShaderBytes[]
		{
			#include "Image.vert.bin"
		};


		uint8_t fragmentShaderBytes[]
		{
			#include "Image.frag.bin"
		};


		return Vulkan::Pipeline::Builder(*GetRenderPass())
			.WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT,
							 Vulkan::Shader::Compile(*mQueue->GetDevice(), Core::IO::DynamicByteBuffer(vertexShaderBytes, sizeof(vertexShaderBytes))).Unwrap())
			.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
							 Vulkan::Shader::Compile(*mQueue->GetDevice(), Core::IO::DynamicByteBuffer(fragmentShaderBytes, sizeof(fragmentShaderBytes))).Unwrap())
			.WithColorBlending()
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT))
			.WithViewport({0, 0}, GetResolution().AsType<float>())
			.Build();
	}
}
