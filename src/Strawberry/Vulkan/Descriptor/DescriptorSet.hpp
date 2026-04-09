#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Vulkan/Pipeline/PipelineLayout.hpp"
#include <Strawberry/Core/Types/ReflexivePointer.hpp>
// Vulkan
#include <vulkan/vulkan.h>

#include "Strawberry/Vulkan/Error.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class DescriptorPool;
	class Sampler;
	class ImageView;
	class Buffer;


	class DescriptorSet
	{
		friend class CommandBuffer;

	public:
		static Result<DescriptorSet> Allocate(Device& device, const DescriptorSetLayout& layout);
		static Result<DescriptorSet> Allocate(DescriptorPool& pool, const DescriptorSetLayout& layout);


		// DescriptorSet(Device& device, const DescriptorSetLayout& layout);
		// DescriptorSet(DescriptorPool& descriptorPool, const DescriptorSetLayout& layout);
		DescriptorSet(const DescriptorSet& rhs)            = default;
		DescriptorSet& operator=(const DescriptorSet& rhs) = default;
		DescriptorSet(DescriptorSet&& rhs) noexcept;
		DescriptorSet& operator=(DescriptorSet&& rhs) noexcept;
		~DescriptorSet();


		void SetUniformBuffer(uint32_t binding, uint32_t arrayElement, const Buffer& buffer);
		void SetStorageBuffer(uint32_t binding, uint32_t arrayElement, const Buffer& buffer);


		void SetTexture(uint32_t binding, uint32_t arrayElement, const ImageView& imageView, VkImageLayout layout);
		void SetSampler(uint32_t binding, uint32_t arrayElement, const Sampler& sampler, VkImageLayout layout);
		void SetCombinedImageSampler(uint32_t binding, uint32_t arrayElement, const Sampler& sampler, const ImageView& image, VkImageLayout layout);

	private:
		DescriptorSet(VkDescriptorSet set, DescriptorPool& pool);


		VkDescriptorSet                        mDescriptorSet;
		Core::ReflexivePointer<DescriptorPool> mDescriptorPool;
	};
}
