#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Instance;

	class Pipeline;


	class Device
		: public Core::EnableReflexivePointer<Device>
	{
		friend class Pipeline;
		friend class Swapchain;
		friend class Surface;
		friend class Queue;
		friend class CommandBuffer;
		friend class CommandPool;
		friend class Buffer;
		friend class BufferView;
		friend class DeviceMemory;
		friend class Image;
		friend class ShaderModule;
		friend class Framebuffer;
		friend class Fence;
		friend class Sampler;
		friend class RenderPass;


	public:
		explicit Device(const Instance& instance);
		Device(const Device& rhs) = delete;
		Device& operator=(const Device& rhs) = delete;
		Device(Device&& rhs) noexcept;
		Device& operator=(Device&& rhs) noexcept ;
		~Device();


		template <std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


	private:
		VkInstance mInstance{};
		VkPhysicalDevice mPhysicalDevice;
		VkDevice mDevice;
		uint32_t mQueueFamilyIndex{};
	};
}
