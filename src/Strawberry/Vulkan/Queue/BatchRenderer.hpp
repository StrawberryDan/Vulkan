#pragma once
#include "Strawberry/Vulkan/Queue/Batch.hpp"
#include <deque>

namespace Strawberry::Vulkan
{
	class BatchRenderer
	{
	public:
		void Enqueue(Batch&& batch);


		template <std::ranges::range Range> requires (std::convertible_to<std::ranges::range_value_t<Range>, Batch>)
		void Enqueue(Range&& range)
		{
			for (auto&& item : range)
			{
				this->Enqueue(std::move(item));
			}
		}


		void WriteQueue(CommandBuffer& buffer);


		void NewBatch();


	private:
		static void ApplyBatchTransition(CommandBuffer& buffer, const Batch& batch, const Core::Optional<const Batch*>& lastBatch);


		std::deque<Batch> mBatches;
	};
}
