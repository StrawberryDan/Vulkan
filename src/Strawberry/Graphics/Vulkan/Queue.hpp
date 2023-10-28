#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Fence.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include <Strawberry/Core/Types/ReflexivePointer.hpp>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;
	class CommandBuffer;


	class Queue
		: public Core::EnableReflexivePointer<Queue>
	{
		friend class Swapchain;


	public:
		explicit Queue(const Device& device);
		Queue(const Queue& rhs) = delete;
		Queue& operator=(const Queue& rhs) = delete;
		Queue(Queue&& rhs) noexcept;
		Queue& operator=(Queue&& rhs);
		~Queue();


		template <std::movable T, typename... Args>
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


		void Submit(const CommandBuffer& commandBuffer);


		Core::ReflexivePointer<Device> GetDevice() const;


		uint32_t GetFamilyIndex() const;


	private:
		VkQueue mQueue;
		uint32_t mFamilyIndex;
		Core::ReflexivePointer<Device> mDevice;
		Fence mSubmissionFence;
	};
}
