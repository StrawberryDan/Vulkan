#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>
#include <utility>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;


	class CommandPool
	{
		friend class CommandBuffer;


	public:
		explicit CommandPool(const Device& device, bool resetBit);
		CommandPool(const CommandPool& rhs) = delete;
		CommandPool& operator=(const CommandPool& rhs) = delete;
		CommandPool(CommandPool&& rhs) noexcept ;
		CommandPool& operator=(CommandPool&& rhs);
		~CommandPool();


		template<std::movable T, typename... Args>
		requires (std::constructible_from<T, const CommandPool&, Args...>)
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		VkCommandPool mCommandPool;
		VkDevice mDevice;
		uint32_t mQueueFamilyIndex;
	};
}
