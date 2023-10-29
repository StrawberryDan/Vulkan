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
#include "Strawberry/Graphics/Vulkan/ShaderModule.hpp"
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
	SpriteRenderer::SpriteRenderer(const Vulkan::Queue& queue, Core::Math::Vec2f viewportSize, VkFilter minFilter, VkFilter magFilter)
		: mQueue(queue)
		, mViewportSize(viewportSize)
		, mRenderPass(queue.GetDevice()->Create<Vulkan::RenderPass::Builder>()
		    .WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE)
			.WithSubpass(Vulkan::SubpassDescription().WithColorAttachment(0))
			.Build())
		, mPipeline(CreatePipeline())
		, mCommandBuffer(mQueue->Create<Vulkan::CommandBuffer>())
		, mCameraBuffer(*queue.GetDevice(), 16 * sizeof(float), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		, mMinFilter(minFilter)
		, mMagFilter(magFilter)
		, mSampler(*queue.GetDevice(), mMinFilter, mMagFilter)
	{

	}


	void SpriteRenderer::Draw(Vulkan::Framebuffer& framebuffer, const Sprite& sprite)
	{
		Core::Math::Mat4f cameraMatrix = Core::Math::Translate<float>(-1.0, -1.0, 0.0) * Core::Math::Scale<float>(2.0 / mViewportSize[0], 2.0 / mViewportSize[1], 1.0);
		mCameraBuffer.SetData(Core::IO::DynamicByteBuffer(cameraMatrix));
		mPipeline.SetUniformBuffer(mCameraBuffer, 0, 0);
		mPipeline.SetUniformTexture(mSampler, sprite.mSpriteSheet->mImageView, VK_IMAGE_LAYOUT_GENERAL, 1, 0);


		mCommandBuffer.Begin(true);
		mCommandBuffer.BindPipeline(mPipeline);
		mCommandBuffer.BindDescriptorSet(mPipeline, 0);
		mCommandBuffer.BindDescriptorSet(mPipeline, 1);
		mCommandBuffer.BeginRenderPass(*framebuffer.GetRenderPass(), framebuffer);


		mCommandBuffer.PushConstants(mPipeline, VK_SHADER_STAGE_VERTEX_BIT, Core::IO::DynamicByteBuffer(sprite.GetTransform().AsMatrix()), 0);
		mCommandBuffer.Draw(6);


		mCommandBuffer.EndRenderPass();
		mCommandBuffer.End();
		mQueue->Submit(mCommandBuffer);
	}


	Vulkan::Pipeline SpriteRenderer::CreatePipeline()
	{
		auto vertexShader = Vulkan::ShaderModule(*mQueue->GetDevice(),
												 Core::IO::DynamicByteBuffer(vertexShaderCode, sizeof(vertexShaderCode)));
		auto fragmentShader = Vulkan::ShaderModule(*mQueue->GetDevice(),
												 Core::IO::DynamicByteBuffer(fragmentShaderCode, sizeof(fragmentShaderCode)));

		return mRenderPass.Create<Vulkan::Pipeline::Builder>()
			.WithViewport({0, 0}, mViewportSize)
		    .WithPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.WithPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 16 * sizeof(float), 0)
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT))
			.WithDescriptorSetLayout(Vulkan::DescriptorSetLayout()
				.WithBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT))
		    .WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, std::move(vertexShader))
			.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader))
		    .Build();
	}
}