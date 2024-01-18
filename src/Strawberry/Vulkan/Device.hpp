#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "PhysicalDevice.hpp"
#include "Queue.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <vector>
#include <map>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Instance;
	class GraphicsPipeline;


	struct QueueCreateInfo
	{
		uint32_t familyIndex;
		uint32_t count;
	};


	class Device
		: public Core::EnableReflexivePointer<Device>
	{
	public:
		explicit Device(const PhysicalDevice& physicalDevice, std::vector<QueueCreateInfo> queueCreateInfo);
		Device(const Device& rhs) = delete;
		Device& operator=(const Device& rhs) = delete;
		Device(Device&& rhs) noexcept;
		Device& operator=(Device&& rhs) noexcept ;
		~Device();


		operator VkDevice() const;


		[[nodiscard]] Core::ReflexivePointer<Instance>                           GetInstance() const;
		[[nodiscard]] const std::vector<Core::ReflexivePointer<PhysicalDevice>>& GetPhysicalDevices() const;


		[[nodiscard]] Core::ReflexivePointer<Queue>                              GetQueue(uint32_t family, uint32_t index);


	private:
		VkDevice mDevice;
		std::vector<Core::ReflexivePointer<PhysicalDevice>> mPhysicalDevices;


		std::map<uint32_t, std::vector<Queue>> mQueues;
	};
}
