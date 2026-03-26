#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Vulkan/Synchronisation/Fence.hpp"
#include "Strawberry/Vulkan/Queue/CommandPool.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include <future>
#include <Strawberry/Core/Types/ReflexivePointer.hpp>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;
	class CommandBuffer;


	class Queue
			: public Core::EnableReflexivePointer
	{
		friend class Device;
		friend class Swapchain;
		friend class CommandBuffer;

	public:
		Queue(const Queue& rhs)            = delete;
		Queue& operator=(const Queue& rhs) = delete;
		Queue(Queue&& rhs) noexcept;
		Queue& operator=(Queue&& rhs) noexcept;
		~Queue();


		template<typename T, typename... Args>
		T Create(Args&&... args) const
		{
			return T(*this, std::forward<Args>(args)...);
		}


		std::future<void> Submit(const CommandBuffer& commandBuffer);
		void WaitUntilIdle() const;


		Device& GetDevice();
		const Device& GetDevice() const;


		uint32_t GetFamilyIndex() const;

		VkQueueFlags GetFlags() const { return mQueueFlags; }


	protected:
		explicit Queue(Device& device, uint32_t family, uint32_t index, VkQueueFlags queueFlags);


	private:
		VkQueue                        mQueue;
		uint32_t                       mFamilyIndex;
		Core::ReflexivePointer<Device> mDevice;
		VkQueueFlags                   mQueueFlags;
	};
}
