//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "TextRenderer.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
// Strawberry Core
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Core/Math/Matrix.hpp"
#include "Strawberry/Core/Math/Transformations.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	TextRenderer::TextRenderer(Vulkan::Queue& queue, Vulkan::RenderPass& renderPass,  Core::Math::Vec2u resolution)
		: Renderer(queue, renderPass, resolution)
		, mPipeline(CreatePipeline(*GetRenderPass(), GetResolution()))
		, mDescriptorSet(mPipeline.AllocateDescriptorSet(0))
		, mPassConstantsBuffer(*GetQueue()->GetDevice(), sizeof(Core::Math::Mat4f), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mDrawConstantsBuffer(*GetQueue()->GetDevice(), 4 * sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mSampler(*GetQueue()->GetDevice(), VK_FILTER_LINEAR, VK_FILTER_LINEAR)
		, mFragDrawConstantsBuffer(*GetQueue()->GetDevice(), 4 * sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
	{

	}


	void TextRenderer::Draw(const FontFace& fontface, const std::string& string, Core::Math::Vec2i position, Core::Math::Vec4f color)
	{
		Draw(fontface, Core::ToUTF32(std::u8string(string.begin(), string.end())), position, color);
	}


	void TextRenderer::Draw(const FontFace& fontface, const std::u32string& string, Core::Math::Vec2i position,
							Core::Math::Vec4f color)
	{
		for (auto c : string)
		{
			auto image = fontface.GetGlyphBitmap(*GetQueue(), c);
			if (!image)
			{
				position = position + fontface.GetGlyphAdvance(c);
				continue;
			}

			auto imageView = Vulkan::ImageView::Builder(image.Value())
				.WithType(VK_IMAGE_VIEW_TYPE_2D)
				.WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
				.Build();
			mDescriptorSet.SetUniformTexture(mSampler, imageView, VK_IMAGE_LAYOUT_GENERAL, 2);

			Core::IO::DynamicByteBuffer bytes;
			Core::Math::Mat4f viewMatrix = Core::Math::Translate(Core::Math::Vec3f(-1.0, -1.0, 0.0))
			                               * Core::Math::Scale(GetResolution().AsType<float>().Map([](float x) { return 2.0f / x; }).WithAdditionalValues(1.0f));
			bytes.Push(viewMatrix);
			mPassConstantsBuffer.SetData(bytes);
			mDescriptorSet.SetUniformBuffer(mPassConstantsBuffer, 0);
			bytes = Core::IO::DynamicByteBuffer();
			bytes.Push(position.AsType<float>());
			bytes.Push(image->GetSize().AsType<float>().AsSize<2>());
			mDrawConstantsBuffer.SetData(bytes);
			mDescriptorSet.SetUniformBuffer(mDrawConstantsBuffer, 1);
			bytes = Core::IO::DynamicByteBuffer();
			bytes.Push(color);
			mFragDrawConstantsBuffer.SetData(bytes);
			mDescriptorSet.SetUniformBuffer(mFragDrawConstantsBuffer, 3);


			auto commandBuffer = GetQueue()->Create<Vulkan::CommandBuffer>();
			commandBuffer.Begin(true);
			commandBuffer.BindPipeline(mPipeline);
			commandBuffer.BindDescriptorSet(mPipeline, 0, mDescriptorSet);
			commandBuffer.BeginRenderPass(*GetRenderPass(), GetFramebuffer());
			commandBuffer.Draw(4);
			commandBuffer.EndRenderPass();
			commandBuffer.End();
			GetQueue()->Submit(commandBuffer);

			position = position + fontface.GetGlyphAdvance(c);
		}
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
			.WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.WithViewport({0, 0}, renderSize.AsType<float>())
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT))
			.WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, std::move(vertexShader))
			.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader))
			.WithColorBlending()
			.Build();
	}
}
