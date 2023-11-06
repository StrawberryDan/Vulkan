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
namespace Strawberry::Graphics::Vulkan
{
	class Device;



	class DescriptorPool
		: public Core::EnableReflexivePointer<DescriptorPool>
	{
		friend class DescriptorSet;


	public:
		DescriptorPool(const Device& device, VkDescriptorPoolCreateFlags flags, uint32_t maxSets, std::vector<VkDescriptorPoolSize> poolSizes);
		DescriptorPool(const DescriptorPool& rhs) = default;
		DescriptorPool& operator=(const DescriptorPool& rhs) = default;
		DescriptorPool(DescriptorPool&& rhs) noexcept;
		DescriptorPool& operator=(DescriptorPool&& rhs) noexcept;
		~DescriptorPool();


		Core::ReflexivePointer<Device> GetDevice() const;


		template <std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


	private:
		VkDescriptorPool mDescriptorPool;
		Core::ReflexivePointer<Device> mDevice;
		VkDescriptorPoolCreateFlags mFlags;
	};
}
