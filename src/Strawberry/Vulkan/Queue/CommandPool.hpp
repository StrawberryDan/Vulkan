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
#include <set>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Queue;
	class CommandBuffer;


	class CommandPool
			: public Core::EnableReflexivePointer
	{
		friend class CommandBuffer;

	public:
		CommandPool(Queue& queue, bool individualReset = false);
		CommandPool(const CommandPool& rhs)            = delete;
		CommandPool& operator=(const CommandPool& rhs) = delete;
		CommandPool(CommandPool&& rhs) noexcept;
		CommandPool& operator=(CommandPool&& rhs);
		~CommandPool();


		void Reset();


		Core::ReflexivePointer<Queue> GetQueue() const;


	private:
		VkCommandPool                                   mCommandPool;
		Core::ReflexivePointer<Queue>                   mQueue;
		std::set<Core::ReflexivePointer<CommandBuffer>> mCommandBuffers;
	};
}
