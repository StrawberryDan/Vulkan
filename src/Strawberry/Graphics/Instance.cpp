//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Instance.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>

//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Instance::Instance()
	{
		VkApplicationInfo applicationInfo{};
		applicationInfo.pNext = nullptr;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);
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
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);
		Core::Assert(result == VK_SUCCESS);
	}


	Instance::~Instance()
	{
		vkDestroyInstance(mInstance, nullptr);
	}


	Instance::Instance(Instance&& rhs)
		: mInstance(std::exchange(rhs.mInstance, nullptr)) {}


	Instance& Instance::operator=(Instance&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}
}