#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;


	class Sampler
	{
		friend class Pipeline;


	public:
		Sampler(const Device& device, VkFilter magFilter, VkFilter minFilter);
		Sampler(const Sampler& rhs) = delete;
		Sampler& operator=(const Sampler& rhs) = delete;
		Sampler(Sampler&& rhs) noexcept;
		Sampler& operator=(Sampler&& rhs) noexcept;
		~Sampler();


	private:
		VkSampler mSampler;
		Core::ReflexivePointer<Device> mDevice;
	};
}
