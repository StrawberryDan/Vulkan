#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include "vulkan/vulkan.h"
// Standard Library
#include <concepts>
#include <utility>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;
	class Surface;


	class Instance
	{
		friend class Device;
		friend class Surface;


	public:
		Instance();
		~Instance();

		Instance(const Instance&) = delete;
		Instance(Instance&&) noexcept ;

		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&) noexcept ;

		template <std::movable T, typename... Args> requires (std::constructible_from<T, const Instance&, Args...>)
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


	private:
		VkInstance mInstance = {};
		VkDebugReportCallbackEXT mDebugReportCallback;
		VkDebugUtilsMessengerEXT mDebugUtilsCallback;


		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
		PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
	};
}
