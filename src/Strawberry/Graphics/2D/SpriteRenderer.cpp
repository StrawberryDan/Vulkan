//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "SpriteRenderer.hpp"
#include "SpriteSheet.hpp"
#include "Strawberry/Graphics/Vulkan/Sampler.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Graphics/Vulkan/CommandPool.hpp"
#include "Strawberry/Graphics/Vulkan/Device.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/Vulkan/Shader.hpp"
// Strawberry Core
#include <Strawberry/Core/Math/Transformations.hpp>


//======================================================================================================================
//  Shader Bytes
//----------------------------------------------------------------------------------------------------------------------
static const uint8_t vertexShaderCode[] =
{
	#include "Sprite.vert.bin"
};


static const uint8_t fragmentShaderCode[] =
{
	#include "Sprite.frag.bin"
};


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	SpriteRenderer::SpriteRenderer(Vulkan::Queue& queue, Core::Math::Vec2u resolution, VkFilter minFilter, VkFilter magFilter)
		: Renderer(queue, CreateRenderPass(queue), resolution)
		, mPipeline(CreatePipeline())
		, mVertexDescriptorSet(mPipeline.AllocateDescriptorSet(0))
		, mCameraBuffer(*queue.GetDevice(), 16 * sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mFragmentDescriptorSet(mPipeline.AllocateDescriptorSet(1))
		, mSpriteSheetBuffer(*queue.GetDevice(), 2 * sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mMinFilter(minFilter)
		, mMagFilter(magFilter)
		, mSampler(*queue.GetDevice(), mMinFilter, mMagFilter, false)
	{

	}


	void SpriteRenderer::Draw(const Sprite& sprite, Transform2D transform)
	{
		Core::Math::Mat4f viewMatrix = Core::Math::Translate(Core::Math::Vec3f(-1.0, -1.0, 0.0)) * Core::Math::Scale(GetResolution().AsType<float>().Map([](auto x) {return 2.0f / x;}).WithAdditionalValues(1.0f));
		mCameraBuffer.SetData(Core::IO::DynamicByteBuffer(viewMatrix));
		Core::Math::Vec2f spriteSize = sprite.mSpriteSheet->GetSpriteSize().AsType<float>();
		mSpriteSheetBuffer.SetData(Core::IO::DynamicByteBuffer(spriteSize));
		mVertexDescriptorSet.SetUniformBuffer(mCameraBuffer, 0);
		mVertexDescriptorSet.SetUniformBuffer(mSpriteSheetBuffer, 1);
		mFragmentDescriptorSet.SetUniformTexture(mSampler, sprite.mSpriteSheet->mImageView, VK_IMAGE_LAYOUT_GENERAL, 0);


		auto commandBuffer = GetQueue()->Create<Vulkan::CommandBuffer>();
		commandBuffer.Begin(true);
		commandBuffer.BindPipeline(mPipeline);
		commandBuffer.BindDescriptorSet(mPipeline, 0, mVertexDescriptorSet);
		commandBuffer.BindDescriptorSet(mPipeline, 1, mFragmentDescriptorSet);
		commandBuffer.ImageMemoryBarrier(sprite.mSpriteSheet->mImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL);
		commandBuffer.BeginRenderPass(*GetRenderPass(), GetFramebuffer());


		commandBuffer.PushConstants(mPipeline, VK_SHADER_STAGE_VERTEX_BIT, Core::IO::DynamicByteBuffer(transform.AsMatrix()), 0);
		commandBuffer.PushConstants(mPipeline, VK_SHADER_STAGE_VERTEX_BIT, Core::IO::DynamicByteBuffer(sprite.mSpriteCoords), sizeof(Core::Math::Mat4f));
		commandBuffer.Draw(6);


		commandBuffer.EndRenderPass();
		commandBuffer.End();
		GetQueue()->Submit(std::move(commandBuffer));
		GetQueue()->WaitUntilIdle();
	}


	Vulkan::RenderPass SpriteRenderer::CreateRenderPass(Vulkan::Queue& queue)
	{
		// Renderpass with 1 color attachment
		return Vulkan::RenderPass::Builder(*queue.GetDevice())
			.WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE)
			.WithSubpass(Vulkan::SubpassDescription().WithColorAttachment(0))
			.Build();
	}


	Vulkan::Pipeline SpriteRenderer::CreatePipeline()
	{
		auto vertexShader = Vulkan::Shader::Compile(*GetQueue()->GetDevice(),
													Core::IO::DynamicByteBuffer(vertexShaderCode, sizeof(vertexShaderCode))).Unwrap();
		auto fragmentShader = Vulkan::Shader::Compile(*GetQueue()->GetDevice(),
													  Core::IO::DynamicByteBuffer(fragmentShaderCode, sizeof(fragmentShaderCode))).Unwrap();

		return GetRenderPass()->Create<Vulkan::Pipeline::Builder>()
			.WithViewport({0, 0}, GetResolution().AsType<float>())
		    .WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.WithPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 16 * sizeof(float) + 2 * sizeof(unsigned int), 0)
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT))
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT))
		    .WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, std::move(vertexShader))
			.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader))
			.WithColorBlending()
		    .Build();
	}
}
