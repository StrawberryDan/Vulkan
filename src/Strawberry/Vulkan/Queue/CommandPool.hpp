#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include <Strawberry/Core/Types/ReflexivePointer.hpp>
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <utility>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Queue;


	class CommandPool
			: public Core::EnableReflexivePointer
	{
		friend class CommandBuffer;

	public:
		CommandPool(Queue& queue, bool individualReset = true);
		CommandPool(const CommandPool& rhs)            = delete;
		CommandPool& operator=(const CommandPool& rhs) = delete;
		CommandPool(CommandPool&& rhs) noexcept;
		CommandPool& operator=(CommandPool&& rhs);
		~CommandPool();


		void Reset();


		Core::ReflexivePointer<Queue> GetQueue() const;

	private:
		VkCommandPool                 mCommandPool;
		Core::ReflexivePointer<Queue> mQueue;
	};
}
