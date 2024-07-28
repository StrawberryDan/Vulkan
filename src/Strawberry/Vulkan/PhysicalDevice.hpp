#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Memory/MemoryTypeCriteria.hpp"
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


	struct MemoryType
	{
		uint32_t              index;
		size_t                heapSize;
		VkMemoryPropertyFlags properties;
	};


	class PhysicalDevice
			: public Core::EnableReflexivePointer
	{
		friend class Instance;
		friend class Surface;
		friend class Device;
		friend class Swapchain;


	public:
		PhysicalDevice(const PhysicalDevice&)            = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&);
		PhysicalDevice& operator=(PhysicalDevice&&);


		Core::ReflexivePointer<Instance> GetInstance() const;


		VkPhysicalDeviceType                        GetType() const;
		const VkPhysicalDeviceProperties&           GetProperties() const;
		const VkPhysicalDeviceLimits&               GetLimits() const;
		const VkPhysicalDeviceFeatures&             GetFeatures() const;
		const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;
		const VkPhysicalDeviceMemoryProperties&     GetMemoryProperties() const;
		const std::vector<VkExtensionProperties>&   GetExtensionProperties() const;


		std::vector<uint32_t>   SearchQueueFamilies(VkQueueFlags flagBits) const;
		std::vector<MemoryType> SearchMemoryTypes(const MemoryTypeCriteria& memoryCriteria) const;


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
