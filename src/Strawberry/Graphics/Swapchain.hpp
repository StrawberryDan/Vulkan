#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Fence.hpp"
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/DynamicValue.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	namespace Window { class Window; }


	class Device;
	class Surface;
	class Framebuffer;
	class Queue;


	class Swapchain
	{
		friend class CommandBuffer;


	public:
		explicit Swapchain(const Device& device, const Surface& surface, Core::Math::Vec2i extents);
		Swapchain(const Swapchain& rhs) = delete;
		Swapchain& operator=(const Swapchain& rhs) = delete;
		Swapchain(Swapchain&& rhs) noexcept;
		Swapchain& operator=(Swapchain&& rhs) noexcept;
		~Swapchain();


		Core::Math::Vec2i GetSize() const;
		VkSurfaceFormatKHR GetSurfaceFormat() const;


		uint32_t GetNextImageIndex();
		VkImage GetNextImage();


		void Present(const Queue& queue);


	protected:
		uint32_t CalculateNextImageIndex();
		VkImage CalculateNextImage();


	private:
		VkSwapchainKHR mSwapchain;

		Core::Math::Vec2i mSize;
		VkSurfaceFormatKHR mFormat;

		Core::ReflexivePointer<Device> mDevice;

		Fence mNextImageFence;

		Core::DynamicValue<uint32_t> mNextImageIndex;
		Core::DynamicValue<VkImage> mNextImage;
	};
}
