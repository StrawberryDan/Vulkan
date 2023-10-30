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
namespace Strawberry::Graphics::Vulkan
{
	static VkBool32 DebuggingCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	                                  VkDebugUtilsMessageTypeFlagsEXT type,
								      const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
								      void* userData)
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
		applicationInfo.pNext = nullptr;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 2, 269);
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pApplicationName = "StrawberryGraphics";
		applicationInfo.pEngineName = "StrawberryGraphics";
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		std::vector<const char*> extensions =
		{
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#if __APPLE__
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
		};

		// Append glfw extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		Core::Assert(glfwExtensions != nullptr);

		for (int i = 0; i < glfwExtensionCount; i++)
		{
			if (!std::any_of(extensions.begin(), extensions.end(),[&](const char* x) { return std::strcmp(x, glfwExtensions[i]) == 0; } ))
			{
				extensions.push_back(glfwExtensions[i]);
			}
		}

		std::vector<const char*> layers
		{
			"VK_LAYER_KHRONOS_validation"
		};


		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
							 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
							 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
							 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
			.pfnUserCallback = DebuggingCallback,
			.pUserData = nullptr,
		};


		VkInstanceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = &messengerCreateInfo,
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
	}


	Instance::~Instance()
	{
		vkDestroyInstance(mInstance, nullptr);
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
}
