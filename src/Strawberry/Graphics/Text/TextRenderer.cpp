//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "TextRenderer.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
// Strawberry Core
#include <Strawberry/Core/UTF.hpp>
#include <Strawberry/Core/Math/Matrix.hpp>
#include <Strawberry/Core/Math/Transformations.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	TextRenderer::TextRenderer(const Vulkan::Queue& queue, Core::Math::Vec2u renderSize)
		: mQueue(queue)
		, mRenderSize(renderSize)
		, mRenderPass(CreateRenderPass(*mQueue->GetDevice()))
		, mPipeline(CreatePipeline(mRenderPass, renderSize))
		, mPassConstantsBuffer(*mQueue->GetDevice(), sizeof(Core::Math::Mat4f), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mDrawConstantsBuffer(*mQueue->GetDevice(), 4 * sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mSampler(*mQueue->GetDevice(), VK_FILTER_LINEAR, VK_FILTER_LINEAR)
		, mDescriptorSet(mPipeline.AllocateDescriptorSet(0))
	{

	}


	void TextRenderer::Draw(const FontFace& fontface, const std::string& string)
	{
		Draw(fontface, Core::ToUTF32(std::u8string(string.begin(), string.end())));
	}


	void TextRenderer::Draw(const FontFace& fontface, const std::u32string& string)
	{
		if (!mFrameBuffer)
		{
			mFrameBuffer = Vulkan::Framebuffer(mRenderPass, mRenderSize);
			mFrameBuffer->GetColorAttachment(0).ClearColor(*mQueue);
		}

		Core::Math::Vec2f position;

		for (auto c : string)
		{
			auto image = fontface.GetGlyphBitmap(*mQueue, c);
			auto imageView = Vulkan::ImageView::Builder(image)
				.WithType(VK_IMAGE_VIEW_TYPE_2D)
				.WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
				.Build();
			mDescriptorSet.SetUniformTexture(mSampler, imageView, VK_IMAGE_LAYOUT_GENERAL, 2);

			Core::IO::DynamicByteBuffer bytes;
			Core::Math::Mat4f viewMatrix = Core::Math::Translate(Core::Math::Vec3f(-1.0, -1.0, 0.0))
			                               * Core::Math::Scale(mRenderSize.AsType<float>().WithAdditionalValues(1.0f).Map([](float x) { return 2.0f / x; }));
			bytes.Push(viewMatrix);
			mPassConstantsBuffer.SetData(bytes);
			mDescriptorSet.SetUniformBuffer(mPassConstantsBuffer, 0);
			bytes = Core::IO::DynamicByteBuffer();
			bytes.Push(position);
			bytes.Push(image.GetSize().AsType<float>().AsSize<2>());
			mDrawConstantsBuffer.SetData(bytes);
			mDescriptorSet.SetUniformBuffer(mDrawConstantsBuffer, 1);


			auto commandBuffer = mQueue->Create<Vulkan::CommandBuffer>();
			commandBuffer.Begin(true);
			commandBuffer.BindPipeline(mPipeline);
			commandBuffer.BindDescriptorSet(mPipeline, 0, mDescriptorSet);
			commandBuffer.BeginRenderPass(mRenderPass, mFrameBuffer.Value());
			commandBuffer.Draw(4);
			commandBuffer.EndRenderPass();
			commandBuffer.End();
			mQueue->Submit(commandBuffer);
		}
	}


	void TextRenderer::SetFramebuffer(Vulkan::Framebuffer framebuffer)
	{
		mFrameBuffer = std::move(framebuffer);
	}


	Vulkan::Framebuffer TextRenderer::GetFramebuffer()
	{
		if (!mFrameBuffer)
		{
			mFrameBuffer = Vulkan::Framebuffer(mRenderPass, mRenderSize);
			mFrameBuffer->GetColorAttachment(0).ClearColor(*mQueue);
		}
		return mFrameBuffer.Unwrap();
	}


	Vulkan::RenderPass TextRenderer::CreateRenderPass(const Vulkan::Device& device)
	{
		return Vulkan::RenderPass::Builder(device)
			.WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE)
			.WithSubpass(Vulkan::SubpassDescription().WithColorAttachment(0))
			.Build();
	}


	Vulkan::Pipeline TextRenderer::CreatePipeline(const Vulkan::RenderPass& renderPass, Core::Math::Vec2u renderSize)
	{
		static uint8_t vertexShaderCode[] = {
			#include "Text.vert.bin"
		};
		Vulkan::Shader vertexShader = Vulkan::Shader::Compile(*renderPass.GetDevice(), {vertexShaderCode, sizeof(vertexShaderCode)}).Unwrap();


		static uint8_t fragmentShaderCode[] = {
			#include "Text.frag.bin"
		};
		Vulkan::Shader fragmentShader = Vulkan::Shader::Compile(*renderPass.GetDevice(), {fragmentShaderCode, sizeof(fragmentShaderCode)}).Unwrap();


		return Vulkan::Pipeline::Builder(renderPass)
			.WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP)
			.WithViewport({0, 0}, renderSize.AsType<float>())
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT))
			.WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, std::move(vertexShader))
			.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader))
			.Build();
	}
}
