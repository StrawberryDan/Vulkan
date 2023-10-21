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


	class Fence
	{
		friend class Swapchain;
		friend class CommandBuffer;


	public:
		Fence(const Device& device);
		Fence(const Fence& rhs) = delete;
		Fence& operator=(const Fence& rhs) = delete;
		Fence(Fence&& rhs) noexcept;
		Fence& operator=(Fence&& rhs) noexcept;
		~Fence();


		void Wait();
		void Reset();

	private:
		VkFence mFence;
		VkDevice mDevice;
	};
}
