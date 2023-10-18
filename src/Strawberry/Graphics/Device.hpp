#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Instance;

	class Pipeline;


	class Device
	{
		friend class Pipeline;


	public:
		Device(const Instance& instance);
		Device(const Device& rhs) = delete;
		Device& operator=(const Device& rhs) = delete;
		Device(Device&& rhs);
		Device& operator=(Device&& rhs);
		~Device();


		template <std::movable T, typename... Args> requires (std::constructible_from<T, const Device&, Args...>)
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		VkInstance mInstance;
		VkPhysicalDevice mPhysicalDevice;
		VkDevice mDevice;
	};
}
