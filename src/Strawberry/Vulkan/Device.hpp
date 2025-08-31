#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/PhysicalDevice.hpp"
#include "Strawberry/Vulkan/Queue.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <vector>
#include <map>


namespace Strawberry::Vulkan
{
	class MultiAllocator;
}

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Allocator;
	class Instance;
	class GraphicsPipeline;


	struct QueueCreateInfo
	{
		uint32_t familyIndex;
		uint32_t count;
	};


	class Device
		: public Core::EnableReflexivePointer
	{
	public:
		explicit Device(const PhysicalDevice& physicalDevice, const VkPhysicalDeviceFeatures& features, std::vector<QueueCreateInfo> queueCreateInfo);
		Device(const Device& rhs)            = delete;
		Device& operator=(const Device& rhs) = delete;
		Device(Device&& rhs) noexcept;
		Device& operator=(Device&& rhs) noexcept;
		~Device();


		operator VkDevice() const;


		void WaitUntilIdle() const;


		[[nodiscard]] Core::ReflexivePointer<Instance>      GetInstance() const;
		[[nodiscard]] const PhysicalDevice&                 GetPhysicalDevice() const;
		[[nodiscard]] Queue&                                GetQueue(uint32_t family, uint32_t index);
		[[nodiscard]] MultiAllocator&                       GetAllocator() const;

	private:
		VkDevice                                            mDevice;
		Core::ReflexivePointer<const PhysicalDevice>        mPhysicalDevice;
		std::map<uint32_t, std::vector<Queue>>              mQueues;
		std::unique_ptr<MultiAllocator>                     mAllocator;
	};
}
