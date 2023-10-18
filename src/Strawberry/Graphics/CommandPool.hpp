#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;

	class CommandPool
	{
	public:
		CommandPool(const Device& device);
		CommandPool(const CommandPool& rhs) = delete;
		CommandPool& operator=(const CommandPool& rhs) = delete;
		CommandPool(CommandPool&& rhs);
		CommandPool& operator=(CommandPool&& rhs);
		~CommandPool();

	private:
		VkCommandPool mCommandPool;
		VkDevice mDevice;
	};
}
