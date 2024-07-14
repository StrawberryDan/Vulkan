//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Instance.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/IO/Logging.hpp"
// GLFW3
#include "GLFW/glfw3.h"
// Standard Library
#include <algorithm>
#include <memory>
#include <vector>
#include <cstring>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	static VkBool32 DebugReportCallback(VkDebugReportFlagsEXT      flags,
	                                    VkDebugReportObjectTypeEXT objectType,
	                                    uint64_t                   object,
	                                    size_t                     location,
	                                    int32_t                    messageCode,
	                                    const char*                pLayerPrefix,
	                                    const char*                pMessage,
	                                    void*                      pUserData)
	{
		switch (flags)
		{
			case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
			case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
				Core::Logging::Debug(pMessage);
				break;
			case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
				Core::Logging::Info(pMessage);
				break;
			case VK_DEBUG_REPORT_WARNING_BIT_EXT:
				Core::Logging::Warning(pMessage);
				break;
			case VK_DEBUG_REPORT_ERROR_BIT_EXT:
				Core::Logging::Error(pMessage);
				Core::DebugBreak();
				break;
			default:
				Core::Unreachable();
		}

		return true;
	}


	static VkBool32 DebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
	                                   VkDebugUtilsMessageTypeFlagsEXT             type,
	                                   const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	                                   void*                                       userData)
	{
		switch (severity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				Core::Logging::Debug(callbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				Core::Logging::Info(callbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				Core::Logging::Warning(callbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				Core::Logging::Error(callbackData->pMessage);
				Core::DebugBreak();
				break;
			default:
				Core::Unreachable();
		}

		return true;
	}


	Instance::Instance()
	{
		VkApplicationInfo applicationInfo{};
		applicationInfo.pNext              = nullptr;
		applicationInfo.apiVersion         = VK_MAKE_VERSION(1, 2, 269);
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pApplicationName   = "StrawberryGraphics";
		applicationInfo.pEngineName        = "StrawberryGraphics";
		applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		std::vector<const char*> extensions =
		{
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#if __APPLE__
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
		};

		// Append glfw extensions
		uint32_t     glfwExtensionCount = 0;
		const char** glfwExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		Core::Assert(glfwExtensions != nullptr);

		for (int i = 0; i < glfwExtensionCount; i++)
		{
			if (!std::any_of(extensions.begin(),
			                 extensions.end(),
			                 [&](const char* x)
			                 {
				                 return std::strcmp(x, glfwExtensions[i]) == 0;
			                 }))
			{
				extensions.push_back(glfwExtensions[i]);
			}
		}

		std::vector<const char*> layers
		{
#if STRAWBERRY_DEBUG
			"VK_LAYER_KHRONOS_validation"
#endif
		};


#if STRAWBERRY_DEBUG
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			.pNext = nullptr,
			.pfnCallback = DebugReportCallback,
			.pUserData = nullptr,
		};
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = &callbackCreateInfo,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
			.pfnUserCallback = DebugUtilsCallback,
			.pUserData = nullptr,
		};
#endif

		VkInstanceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if STRAWBERRY_DEBUG
			.pNext = &messengerCreateInfo,
#else
			.pNext = nullptr,
#endif
#if __APPLE__
			.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#else
			.flags = 0,
#endif
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};


		VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);
		Core::Assert(result == VK_SUCCESS);

#if STRAWBERRY_DEBUG
		vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
			vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
		vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(
			mInstance,
			"vkDestroyDebugReportCallbackEXT"));
		vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT"));
		vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
			mInstance,
			"vkDestroyDebugUtilsMessengerEXT"));

		messengerCreateInfo.pNext = nullptr;
		Core::AssertEQ(vkCreateDebugUtilsMessengerEXT(mInstance, &messengerCreateInfo, nullptr, &mDebugUtilsCallback),
		               VK_SUCCESS);

		Core::AssertEQ(
			vkCreateDebugReportCallbackEXT(mInstance, &callbackCreateInfo, nullptr, &mDebugReportCallback),
			VK_SUCCESS);
#endif
	}


	Instance::~Instance()
	{
		if (mInstance)
		{
#if STRAWBERRY_DEBUG
			vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugUtilsCallback, nullptr);
			vkDestroyDebugReportCallbackEXT(mInstance, mDebugReportCallback, nullptr);
#endif
			vkDestroyInstance(mInstance, nullptr);
		}
	}


	Instance::Instance(Instance&& rhs) noexcept
		: mInstance(std::exchange(rhs.mInstance, nullptr)) {}


	Instance& Instance::operator=(Instance&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	const std::vector<PhysicalDevice>& Instance::GetPhysicalDevices() const
	{
		static const auto deviceSort = [](PhysicalDevice& a, PhysicalDevice& b) -> bool
		{
			// Prioritise discrete GPUs
			if (b.GetType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return true;

			return false;
		};

		if (!mPhysicalDevices)
		{
			uint32_t count = 0;
			Core::AssertEQ(vkEnumeratePhysicalDevices(mInstance, &count, nullptr), VK_SUCCESS);
			std::vector<VkPhysicalDevice> devices(count);
			Core::AssertEQ(vkEnumeratePhysicalDevices(mInstance, &count, devices.data()), VK_SUCCESS);

			mPhysicalDevices.Emplace();
			mPhysicalDevices->reserve(count);
			for (auto deviceHandle: devices)
			{
				PhysicalDevice device(*this, deviceHandle);
				mPhysicalDevices->emplace_back(std::move(device));
			}

			std::sort(mPhysicalDevices->begin(), mPhysicalDevices->end(), deviceSort);
		}

		return mPhysicalDevices.Value();
	}
}
