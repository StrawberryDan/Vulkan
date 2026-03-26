#pragma once
#include "Strawberry/Vulkan/Queue/Batch.hpp"
#include <deque>

namespace Strawberry::Vulkan
{
	class BatchRenderer
	{
	public:
		void Enqueue(Batch&& batch);


		void WriteQueue(CommandBuffer& buffer);


		void NewBatch();


	private:
		static void ApplyBatchTransition(CommandBuffer& buffer, const Batch& batch, const Core::Optional<const Batch*>& lastBatch);


		std::deque<Batch> mBatches;
	};
}
