#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include <Strawberry/Core/Types/ReflexivePointer.hpp>
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <vector>


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
		DescriptorSet(const DescriptorPool& descriptorPool, VkDescriptorSetLayout layout);
		DescriptorSet(const DescriptorSet& rhs)            = default;
		DescriptorSet& operator=(const DescriptorSet& rhs) = default;
		DescriptorSet(DescriptorSet&& rhs) noexcept;
		DescriptorSet& operator=(DescriptorSet&& rhs) noexcept;
		~DescriptorSet();


		void SetUniformBuffer(uint32_t binding, uint32_t arrayElement, const Buffer& buffer);
		void SetStorageBuffer(uint32_t binding, uint32_t arrayElement, const Buffer& buffer);


		void SetUniformTexture(uint32_t binding, uint32_t arrayElement, const Sampler& sampler, const ImageView& image, VkImageLayout layout);

	private:
		VkDescriptorSet                        mDescriptorSet;
		Core::ReflexivePointer<DescriptorPool> mDescriptorPool;
	};
}
