//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "SpriteRenderer.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Graphics/Vulkan/CommandPool.hpp"
#include "Strawberry/Graphics/Vulkan/Device.hpp"
#include "Strawberry/Graphics/Vulkan/RenderPass.hpp"
#include "Strawberry/Graphics/Vulkan/ShaderModule.hpp"


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
	SpriteRenderer::SpriteRenderer(const Vulkan::Queue& queue, Core::Math::Vec2f viewportSize)
		: mQueue(queue)
		, mViewportSize(viewportSize)
		, mRenderPass(queue.GetDevice()->Create<Vulkan::RenderPass::Builder>()
		    .WithColorAttachment(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE)
			.WithSubpass(Vulkan::SubpassDescription().WithColorAttachment(0))
			.Build())
		, mPipeline(CreatePipeline())
		, mCommandPool(queue.Create<Vulkan::CommandPool>(false))
	{

	}


	void SpriteRenderer::BeginRenderPass(Vulkan::Framebuffer& framebuffer)
	{
		Core::Assert(!mRenderPassBuffer.HasValue());
		mRenderPassBuffer = mCommandPool.Create<Vulkan::CommandBuffer>();
		mRenderPassBuffer->Begin(true);
		mRenderPassBuffer->BindPipeline(mPipeline);
		mRenderPassBuffer->BeginRenderPass(*framebuffer.GetRenderPass(), framebuffer);
	}


	void SpriteRenderer::EndRenderPass()
	{
		Core::Assert(mRenderPassBuffer.HasValue());
		mRenderPassBuffer->EndRenderPass();
		mRenderPassBuffer->End();
		mQueue->Submit(mRenderPassBuffer.Unwrap());
	}


	void SpriteRenderer::Draw(const Sprite& sprite)
	{
		Core::Assert(mRenderPassBuffer.HasValue());
		mRenderPassBuffer->Draw(6);
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
		    .WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT, std::move(vertexShader))
			.WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader))
		    .Build();
	}
}
