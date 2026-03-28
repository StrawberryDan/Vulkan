//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Descriptor/DescriptorPool.hpp"
#include "Strawberry/Vulkan/Descriptor/DescriptorSet.hpp"
#include "Strawberry/Vulkan/Descriptor/Sampler.hpp"
#include "Strawberry/Vulkan/Device/Device.hpp"
#include "Strawberry/Vulkan/Resource/Buffer.hpp"
#include "Strawberry/Vulkan/Resource/ImageView.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	DescriptorSet::DescriptorSet(Device& device, const DescriptorSetLayout& layout)
		: DescriptorSet(device.AllocateDescriptorSet(layout))
	{}


	DescriptorSet::DescriptorSet(DescriptorPool& descriptorPool, const DescriptorSetLayout& layout)
		: mDescriptorSet(VK_NULL_HANDLE)
		, mDescriptorPool(descriptorPool)
	{
		VkDescriptorSetLayout layoutHandle = layout.Handle();

		VkDescriptorSetAllocateInfo allocateInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = mDescriptorPool->mDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &layoutHandle,
		};

		Core::AssertEQ(
			vkAllocateDescriptorSets(
				mDescriptorPool->GetDevice()->Handle(),
				&allocateInfo,
				&mDescriptorSet),
			VK_SUCCESS);
	}


	DescriptorSet::DescriptorSet(DescriptorSet&& rhs) noexcept
		: mDescriptorSet(std::exchange(rhs.mDescriptorSet, VK_NULL_HANDLE))
		, mDescriptorPool(std::move(rhs.mDescriptorPool)) {}


	DescriptorSet& DescriptorSet::operator=(DescriptorSet&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	DescriptorSet::~DescriptorSet()
	{
		if (mDescriptorSet && mDescriptorPool && mDescriptorPool->mFlags & VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
		{
			Core::AssertEQ(
				vkFreeDescriptorSets(mDescriptorPool->GetDevice()->Handle(), mDescriptorPool->mDescriptorPool, 1, &mDescriptorSet),
				VK_SUCCESS);
		}
	}


	void DescriptorSet::SetUniformBuffer(uint32_t binding, uint32_t arrayElement, const Buffer& buffer)
	{
		VkDescriptorBufferInfo bufferInfo{
			.buffer = buffer,
			.offset = 0,
			.range = buffer.GetSize()
		};
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.dstArrayElement = arrayElement,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pImageInfo = nullptr,
			.pBufferInfo = &bufferInfo,
			.pTexelBufferView = nullptr,
		};
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->Handle(), 1, &write, 0, nullptr);
	}


	void DescriptorSet::SetStorageBuffer(uint32_t binding, uint32_t arrayElement, const Strawberry::Vulkan::Buffer& buffer)
	{
		VkDescriptorBufferInfo bufferInfo{
			.buffer = buffer,
			.offset = 0,
			.range = buffer.GetSize()
		};
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.dstArrayElement = arrayElement,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pImageInfo = nullptr,
			.pBufferInfo = &bufferInfo,
			.pTexelBufferView = nullptr,
		};
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->Handle(), 1, &write, 0, nullptr);
	}


	void DescriptorSet::SetTexture(uint32_t binding, uint32_t arrayElement, const ImageView& imageView, VkImageLayout layout)
	{
		VkDescriptorImageInfo imageInfo{
			.sampler = VK_NULL_HANDLE,
			.imageView = imageView,
			.imageLayout = layout,
		};
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.dstArrayElement = arrayElement,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImageInfo = &imageInfo,
			.pBufferInfo = nullptr,
			.pTexelBufferView = nullptr,
		};
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->Handle(), 1, &write, 0, nullptr);
	}

	void DescriptorSet::SetSampler(uint32_t binding, uint32_t arrayElement, const Sampler& sampler, VkImageLayout layout)
	{
		VkDescriptorImageInfo imageInfo{
			.sampler = sampler,
			.imageView = VK_NULL_HANDLE,
			.imageLayout = layout,
		};
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.dstArrayElement = arrayElement,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.pImageInfo = &imageInfo,
			.pBufferInfo = nullptr,
			.pTexelBufferView = nullptr,
		};
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->Handle(), 1, &write, 0, nullptr);
	}


	void DescriptorSet::SetCombinedImageSampler(uint32_t         binding,
												uint32_t         arrayElement,
												const Sampler&   sampler,
												const ImageView& image,
												VkImageLayout    layout)
	{
		VkDescriptorImageInfo imageInfo{
			.sampler = sampler,
			.imageView = image,
			.imageLayout = layout,
		};
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.dstArrayElement = arrayElement,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &imageInfo,
			.pBufferInfo = nullptr,
			.pTexelBufferView = nullptr,
		};
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->Handle(), 1, &write, 0, nullptr);
	}
}
