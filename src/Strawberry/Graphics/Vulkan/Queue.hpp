#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Fence.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include <Strawberry/Core/Types/ReflexivePointer.hpp>
#include <Strawberry/Core/Types/Uninitialised.hpp>
#include <Strawberry/Core/Types/Optional.hpp>


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
		friend class CommandBuffer;


	public:
		explicit Queue(const Device& device);
		Queue(const Queue& rhs) = delete;
		Queue& operator=(const Queue& rhs) = delete;
		Queue(Queue&& rhs) noexcept;
		Queue& operator=(Queue&& rhs);
		~Queue();


		template <typename T, typename... Args>
		T Create(Args&&... args) const { return T(*this, std::forward<Args&&>(args)...); }


		void Submit(CommandBuffer commandBuffer);
		void Wait();


		Core::ReflexivePointer<Device> GetDevice() const;


		uint32_t GetFamilyIndex() const;


	private:
		VkQueue mQueue;
		uint32_t mFamilyIndex;
		Core::ReflexivePointer<Device> mDevice;
		Fence mSubmissionFence;
		Core::Uninitialised<CommandPool> mCommandPool;
		bool mShouldWait = false;
		Core::Optional<CommandBuffer> mCommandBuffer;
	};
}
