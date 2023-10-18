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


	class Queue
	{
	public:
		explicit Queue(const Device& device);
		Queue(const Queue& rhs) = delete;
		Queue& operator=(const Queue& rhs) = delete;
		Queue(Queue&& rhs) noexcept;
		Queue& operator=(Queue&& rhs);
		~Queue() = default;


	private:
		VkQueue mQueue;
	};
}
