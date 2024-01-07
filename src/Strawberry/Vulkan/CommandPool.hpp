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
		: public Core::EnableReflexivePointer<CommandPool>
	{
		friend class CommandBuffer;


	public:
		CommandPool(const Queue& queue, bool resetBit = true);
		CommandPool(const CommandPool& rhs) = delete;
		CommandPool& operator=(const CommandPool& rhs) = delete;
		CommandPool(CommandPool&& rhs) noexcept ;
		CommandPool& operator=(CommandPool&& rhs);
		~CommandPool();


		Core::ReflexivePointer<Queue> GetQueue() const;


		template<std::movable T, typename... Args>
		requires (std::constructible_from<T, const CommandPool&, Args...>)
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


	private:
		VkCommandPool mCommandPool;
		Core::ReflexivePointer<Queue> mQueue;
	};
}
