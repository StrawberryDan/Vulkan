//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "DescriptorSet.hpp"
#include "DescriptorPool.hpp"
#include "Device.hpp"
#include "Image.hpp"
#include "Sampler.hpp"
#include "ImageView.hpp"
#include "Buffer.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	DescriptorSet::DescriptorSet(const DescriptorPool& descriptorPool, VkDescriptorSetLayout layout)
		: mDescriptorSet(VK_NULL_HANDLE)
		, mDescriptorPool(descriptorPool)
	{
		VkDescriptorSetAllocateInfo allocateInfo {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = mDescriptorPool->mDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &layout,
		};

		Core::AssertEQ(vkAllocateDescriptorSets(mDescriptorPool->GetDevice()->mDevice, &allocateInfo, &mDescriptorSet),
					   VK_SUCCESS);
	}


	DescriptorSet::DescriptorSet(DescriptorSet&& rhs) noexcept
		: mDescriptorSet(std::exchange(rhs.mDescriptorSet, VK_NULL_HANDLE))
		, mDescriptorPool(std::move(rhs.mDescriptorPool))
	{

	}


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
			vkFreeDescriptorSets(mDescriptorPool->GetDevice()->mDevice, mDescriptorPool->mDescriptorPool, 1, &mDescriptorSet);
		}
	}


	void DescriptorSet::SetUniformBuffer(const Vulkan::Buffer& buffer, uint32_t binding, uint32_t arrayElement)
	{
		VkDescriptorBufferInfo bufferInfo {
			.buffer = buffer.mBuffer,
			.offset = 0,
			.range = buffer.GetSize()
		};
		VkWriteDescriptorSet write {
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
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->mDevice, 1, &write, 0, nullptr);
	}


	void DescriptorSet::SetUniformTexture(const Sampler& sampler, const ImageView& image, VkImageLayout layout, uint32_t binding, uint32_t arrayElement)
	{
		VkDescriptorImageInfo imageInfo {
			.sampler = sampler.mSampler,
			.imageView = image.mImageView,
			.imageLayout = layout,
		};
		VkWriteDescriptorSet write {
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
		vkUpdateDescriptorSets(mDescriptorPool->GetDevice()->mDevice, 1, &write, 0, nullptr);
	}
}
