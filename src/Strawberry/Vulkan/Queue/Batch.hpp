#pragma once
#include "Strawberry/Vulkan/Resource/Buffer.hpp"
#include "Strawberry/Core/Types/ValOrPtr.hpp"
#include <map>


namespace Strawberry::Vulkan
{
	class Buffer;
	class DescriptorSet;
	class GraphicsPipeline;

	class Batch
	{
	public:
		friend class BatchRenderer;


		Batch(const Batch&) = delete;
		Batch& operator=(const Batch&) = delete;
		Batch(Batch&&) = default;
		Batch& operator=(Batch&&) = default;


		Batch(GraphicsPipeline& pipeline)
			: mGraphicsPipeline(&pipeline)
		{}

		Batch& WithDescriptorSet(unsigned int index, Core::ValOrPtr<DescriptorSet> descriptorSet)
		{
			mDescriptorSets.emplace(index, std::move(descriptorSet));
			return *this;
		}

		Batch& WithVertexBuffer(unsigned int index, Core::ValOrPtr<Buffer> buffer)
		{
			mVertexBuffers.emplace(index, std::move(buffer));
			return *this;
		}

		Batch& WithIndexBuffer(Core::ValOrPtr<Buffer> buffer)
		{
			mIndexBuffer.Emplace(std::move(buffer));
			return *this;
		}

		Batch& WithVertexCount(unsigned int vertexCount)
		{
			mVertexCount = vertexCount;
			return *this;
		}

		Batch& WithInstanceCount(unsigned int instanceCount)
		{
			mInstanceCount = instanceCount;
			return *this;
		}

	private:
		/// Pipeline to use to render batch
		GraphicsPipeline* mGraphicsPipeline = nullptr;
		/// Descriptor sets to use to to render batch
		std::map<unsigned int, Core::ValOrPtr<DescriptorSet>> mDescriptorSets;
		/// Vertex Buffers used to render batch
		std::map<unsigned int, Core::ValOrPtr<Buffer>> mVertexBuffers;
		/// Potential Index Buffer to use render batch
		Core::Optional<Core::ValOrPtr<Buffer>> mIndexBuffer;

		/// Vertex count or index count if using index buffer.
		unsigned int mVertexCount   = 0;
		/// Number of instances to render.
		unsigned int mInstanceCount = 1;
		/// Offset into vertices at which to start rendering.
		unsigned int mFirstVertex   = 0;
		/// Offset into indices at which to start rendering.
		unsigned int mFirstInstance = 0;
	};
}

