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
	class CommandPool;


	class CommandBuffer
	{
	public:
		explicit CommandBuffer(const CommandPool& commandPool);
		CommandBuffer(const CommandBuffer& rhs) = delete;
		CommandBuffer& operator=(const CommandBuffer& rhs) = delete;
		CommandBuffer(CommandBuffer&& rhs) noexcept ;
		CommandBuffer& operator=(CommandBuffer&& rhs);
		~CommandBuffer();


		void Begin();
		void End();


	private:
		VkCommandBuffer mCommandBuffer;
		VkCommandPool mCommandPool;
		VkDevice mDevice;
	};
}
