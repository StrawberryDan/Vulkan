#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include "vulkan/vulkan.h"
// Standard Library
#include <concepts>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;


	class Instance
	{
		friend class Device;


	public:
		Instance();
		~Instance();

		Instance(const Instance&) = delete;
		Instance(Instance&&);

		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&);

		template <std::movable T, typename... Args> requires (std::constructible_from<T, const Instance&, Args...>)
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }
	private:
		VkInstance mInstance = {};
	};
}
