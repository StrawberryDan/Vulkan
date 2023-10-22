#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;


	class RenderPass
		: public Core::EnableReflexivePointer<RenderPass>
	{
		friend class CommandBuffer;
		friend class Pipeline;
		friend class Framebuffer;


	public:
		RenderPass(const Device& device);
		RenderPass(const RenderPass& rhs) = delete;
		RenderPass& operator=(const RenderPass& rhs) = delete;
		RenderPass(RenderPass&& rhs) noexcept;
		RenderPass& operator=(RenderPass&& rhs) noexcept;
		~RenderPass();


		template <std::movable T, typename... Args>
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		VkRenderPass mRenderPass;
		Core::ReflexivePointer<Device> mDevice;
	};
} // Vulkan
