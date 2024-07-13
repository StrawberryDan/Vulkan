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
		Queue(const Queue& rhs) = delete;
		Queue& operator=(const Queue& rhs) = delete;
		Queue(Queue&& rhs) noexcept;
		Queue& operator=(Queue&& rhs);
		~Queue();


		template <typename T, typename... Args>
		T Create(Args&&... args) const { return T(*this, std::forward<Args&&>(args)...); }


		void Submit(const CommandBuffer& commandBuffer);
		void WaitUntilIdle();


		Core::ReflexivePointer<Device> GetDevice() const;


		uint32_t GetFamilyIndex() const;


	protected:
		explicit Queue(const Device& device, uint32_t family, uint32_t index);


	private:
		VkQueue mQueue;
		uint32_t mFamilyIndex;
		Core::ReflexivePointer<Device> mDevice;
	};
}
