#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Resource/Image.hpp"
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Foreward Declarations
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Window
{
	class Window;
}


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;
	class Surface;
	class Framebuffer;
	class Queue;


	class Swapchain
	{
		friend class CommandBuffer;

	public:
		Swapchain(Queue& queue, Surface& surface, Core::Math::Vec2i extents, VkPresentModeKHR presentMode);
		Swapchain(const Swapchain& rhs)            = delete;
		Swapchain& operator=(const Swapchain& rhs) = delete;
		Swapchain(Swapchain&& rhs) noexcept;
		Swapchain& operator=(Swapchain&& rhs) noexcept;
		~Swapchain();


		[[nodiscard]] Core::Math::Vec2i  GetSize() const;
		[[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat() const;


		Core::Optional<uint32_t> GetNextImageIndex();
		Core::Optional<uint32_t> WaitForNextImageIndex();


		Core::Optional<Image*> GetNextImage();
		Core::Optional<Image*> WaitForNextImage();


		void Present();


		bool IsOutOfDate() const;

	private:
		VkSwapchainKHR mSwapchain;

		Core::ReflexivePointer<Queue> mQueue;

		Core::Math::Vec2i  mSize;
		VkSurfaceFormatKHR mFormat;

		std::vector<Image>       mImages;
		Core::Optional<uint32_t> mNextImageIndex;

		bool mIsOutOfDate = false;
	};
}
