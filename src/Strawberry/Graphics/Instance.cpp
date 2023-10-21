//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Instance.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// GLFW3
#include <GLFW/glfw3.h>
// Standard Library
#include <algorithm>
#include <memory>
#include <vector>
#include <cstring>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Instance::Instance()
	{
		VkApplicationInfo applicationInfo{};
		applicationInfo.pNext = nullptr;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 3, 268);
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pApplicationName = "StrawberryGraphics";
		applicationInfo.pEngineName = "StrawberryGraphics";
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		std::vector<const char*> extensions =
		{
#if __APPLE__
			"VK_KHR_portability_enumeration"
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

		VkInstanceCreateInfo createInfo{};
		createInfo.pNext = nullptr;
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#if __APPLE__
		createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
		createInfo.flags = 0;
#endif
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = layers.size();
		createInfo.ppEnabledLayerNames = layers.data();

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
