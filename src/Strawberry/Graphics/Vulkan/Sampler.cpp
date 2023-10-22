//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Sampler.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	Sampler::Sampler(const Device& device, VkFilter magFilter, VkFilter minFilter)
		: mDevice(device)
	{
		VkSamplerCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = magFilter,
			.minFilter = minFilter,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0.0,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 0.0,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_NEVER,
			.minLod = 0.0,
			.maxLod = 0.0,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		};
		Core::AssertEQ(vkCreateSampler(mDevice->mDevice, &createInfo, nullptr, &mSampler), VK_SUCCESS);
	}


	Sampler::Sampler(Sampler&& rhs) noexcept
		: mSampler(std::exchange(rhs.mSampler, nullptr))
		, mDevice(std::move(rhs.mDevice))
	{

	}


	Sampler& Sampler::operator=(Sampler&& rhs) noexcept
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Sampler::~Sampler()
	{
		if (mSampler)
		{
			vkDestroySampler(mDevice->mDevice, mSampler, nullptr);
		}
	}
}
