#pragma once
#include "PipelineLayout.hpp"
#include "Shader.hpp"


namespace Strawberry::Vulkan
{
	class ComputePipeline
	{
	public:
		class Builder;


		ComputePipeline(const ComputePipeline&) = delete;
		ComputePipeline& operator=(const ComputePipeline&) = delete;


		ComputePipeline(ComputePipeline&&) noexcept;
		ComputePipeline& operator=(ComputePipeline&&) noexcept;

		~ComputePipeline() noexcept;


		operator VkPipeline() const;


		      PipelineLayout& GetLayout();
		const PipelineLayout& GetLayout() const;


	private:
		ComputePipeline(const Device& device, PipelineLayout& layout, VkPipeline&& pipeline);


		VkDevice mDevice;
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;
		VkPipeline mPipeline;
	};


	class ComputePipeline::Builder
	{
	public:
		Builder(const Device& device, PipelineLayout& layout, Shader&& shader);


		template <typename T, typename... Ts>
		Builder& WithShaderSpecializationConstants(T t, Ts... ts)
		{
			VkSpecializationMapEntry entry
			{
				.constantID = static_cast<uint32_t>(mShaderSpecializationEntries.size()),
				.offset = static_cast<uint32_t>(mShaderSpecializationData.Size()),
				.size = sizeof(T),
			};
			mShaderSpecializationEntries.emplace_back(entry);

			mShaderSpecializationData.Push(std::forward<T>(t));


			if constexpr(sizeof...(ts) > 0)
			{
				return WithShaderSpecializationConstants(std::forward<Ts>(ts)...);
			}

			return *this;
		}


		ComputePipeline Build();


	private:
		const Device& mDevice;
		PipelineLayout& mPipelineLayout;
		Shader mShader;


		std::vector<VkSpecializationMapEntry> mShaderSpecializationEntries;
		Core::IO::DynamicByteBuffer mShaderSpecializationData;
	};
}
