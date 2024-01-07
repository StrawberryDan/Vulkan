#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
    class Instance;
    
    
	class PhysicalDevice
		: public Core::EnableReflexivePointer<PhysicalDevice>
	{
		friend class Instance;
		friend class Surface;
		friend class Device;
		friend class Swapchain;


	public:
		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&);
		PhysicalDevice& operator=(PhysicalDevice&&);


		Core::ReflexivePointer<Instance>            GetInstance() const;


		VkPhysicalDeviceType                        GetType() const;
		const VkPhysicalDeviceProperties&           GetProperties() const;
		const VkPhysicalDeviceLimits&               GetLimits() const;
		const VkPhysicalDeviceFeatures&             GetFeatures() const;
		const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;
		const VkPhysicalDeviceMemoryProperties      GetMemoryProperties() const;
		const std::vector<VkExtensionProperties>&   GetExtensionProperties() const;


		std::vector<uint32_t>                       SearchQueueFamilies(std::underlying_type_t<VkQueueFlagBits> flagBits) const;


	protected:
		PhysicalDevice(const Instance& instance, VkPhysicalDevice rawHandle);


	private:
		VkPhysicalDevice mPhysicalDevice;


		Core::ReflexivePointer<Instance> mInstance;


		mutable Core::Optional<VkPhysicalDeviceProperties>           mProperties;
		mutable Core::Optional<VkPhysicalDeviceFeatures>             mFeatures;
		mutable Core::Optional<std::vector<VkQueueFamilyProperties>> mQueueFamilyProperties;
		mutable Core::Optional<VkPhysicalDeviceMemoryProperties>     mMemoryProperties;
		mutable Core::Optional<std::vector<VkExtensionProperties>>   mExtensionProperties;
	};
}
