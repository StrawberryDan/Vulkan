#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <vector>


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
		friend class Swapchain;
		friend class Surface;
		friend class Queue;
		friend class CommandPool;
		friend class Buffer;
		friend class BufferView;
		friend class DeviceMemory;
		friend class Image;
		friend class ShaderModule;
		friend class Framebuffer;

	public:
		explicit Device(const Instance& instance);
		Device(const Device& rhs) = delete;
		Device& operator=(const Device& rhs) = delete;
		Device(Device&& rhs) noexcept;
		Device& operator=(Device&& rhs) noexcept ;
		~Device();


		template <std::movable T, typename... Args>
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		VkInstance mInstance{};
		VkPhysicalDevice mPhysicalDevice;
		VkDevice mDevice;
		uint32_t mQueueFamilyIndex{};
	};
}
