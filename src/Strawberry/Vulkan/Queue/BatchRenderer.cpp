#include "Strawberry/Vulkan/Queue/BatchRenderer.hpp"

#include "CommandBuffer.hpp"


namespace Strawberry::Vulkan
{
	void BatchRenderer::Enqueue(Batch&& batch)
	{
		mBatches.emplace_back(std::move(batch));
	}


	void BatchRenderer::WriteQueue(CommandBuffer& buffer)
	{
		Core::Optional<const Batch*> lastBatch;
		for (const auto& batch : mBatches)
		{
			ApplyBatchTransition(buffer, batch, lastBatch);

			if (batch.mIndexBuffer)
			{
				buffer.DrawIndexed(batch.mVertexCount, batch.mInstanceCount, batch.mFirstVertex, batch.mFirstInstance);
			}
			else
			{
				buffer.Draw(batch.mVertexCount, batch.mInstanceCount, batch.mFirstVertex, batch.mFirstInstance);
			}

			lastBatch = &batch;
		}
	}


	void BatchRenderer::NewBatch()
	{
		mBatches.clear();
	}


	void BatchRenderer::ApplyBatchTransition(CommandBuffer& buffer, const Batch& batch, const Core::Optional<const Batch*>& lastBatch)
	{
		if (batch.mGraphicsPipeline != lastBatch.Map([] (const auto& x) { return x->mGraphicsPipeline; } ))
		{
			buffer.BindPipeline(*batch.mGraphicsPipeline);
		}


		for (const auto& [index, descriptorSet] : batch.mDescriptorSets)
		{
			if (!lastBatch.HasValue() || (!lastBatch->mDescriptorSets.contains(index) || descriptorSet != lastBatch->mDescriptorSets.at(index)))
			{
				buffer.BindDescriptorSet(*batch.mGraphicsPipeline, index, descriptorSet.Resolve());
			}
		}

		for (const auto& [index, vertexBuffer] : batch.mVertexBuffers)
		{
			if (!lastBatch.HasValue() || (!lastBatch->mVertexBuffers.contains(index) || vertexBuffer != lastBatch->mVertexBuffers.at(index)))
			{
				buffer.BindVertexBuffer(index, vertexBuffer.Resolve());
			}
		}
	}
}
