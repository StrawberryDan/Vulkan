#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Device/PhysicalDevice.hpp"
#include "Strawberry/Vulkan/Queue/Queue.hpp"
#include "Strawberry/Vulkan/Descriptor/DescriptorSet.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <vector>
#include <map>




//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Allocator;
	class Instance;
	class GraphicsPipeline;
	class DescriptorPoolAllocator;
	class PolyAllocator;
	class DescriptorSetLayout;


	struct QueueCreateInfo
	{
		uint32_t familyIndex;
		uint32_t count;
	};


	struct QueueCriteria
	{
		VkQueueFlags flags;


		QueueCriteria operator|(const QueueCriteria& other) const { return { .flags = flags | other.flags }; }


		static QueueCriteria Graphics();
		static QueueCriteria Transfer();
		static QueueCriteria Compute();
	};


	class Device
			: public Core::EnableReflexivePointer
	{
	public:
		class Builder;

		Device(const Device& rhs) = delete;

		Device& operator=(const Device& rhs) = delete;

		Device(Device&& rhs) noexcept;

		Device& operator=(Device&& rhs) noexcept;

		~Device();


		[[nodiscard]] VkDevice Handle() const;
		explicit operator VkDevice() const;


		void WaitUntilIdle() const;


		[[nodiscard]] Core::ReflexivePointer<Instance> GetInstance() const;

		[[nodiscard]] const PhysicalDevice& GetPhysicalDevice() const;

		[[nodiscard]] Queue& GetQueue(uint32_t family, uint32_t index);

		[[nodiscard]] Queue& GetQueue(const QueueCriteria& queueCriteria);

		[[nodiscard]] PolyAllocator& GetAllocator() const;

		[[nodiscard]] DescriptorSet AllocateDescriptorSet(const DescriptorSetLayout& descriptorSetLayout);

	private:
		explicit Device(const PhysicalDevice&           physicalDevice,
						const VkPhysicalDeviceFeatures& features,
						std::vector<QueueCreateInfo>    queueCreateInfo);


		VkDevice                                     mDevice;
		Core::ReflexivePointer<const PhysicalDevice> mPhysicalDevice;
		std::map<uint32_t, std::vector<Queue>>       mQueues;
		std::unique_ptr<PolyAllocator>               mAllocator;
		std::unique_ptr<DescriptorPoolAllocator>     mDescriptorPoolAllocator;
	};


	class Device::Builder
	{
	public:
		Builder(const PhysicalDevice& physicalDevice);


		Builder& WithFeature(VkBool32 VkPhysicalDeviceFeatures::*Member)
		{
			mFeatures.get()->*Member = VK_TRUE;
			return *this;
		}

		Builder& WithQueue(const QueueCriteria& queueCriteria, unsigned int count = 1);

		Device Build();

	private:
		const PhysicalDevice& device;
		std::unique_ptr<VkPhysicalDeviceFeatures> mFeatures;
		std::vector<QueueCreateInfo> mQueueCreateInfo;
	};
}
