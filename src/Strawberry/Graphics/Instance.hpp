#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "vulkan/vulkan.h"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Instance
	{
	public:
		Instance();
		~Instance();

		Instance(const Instance&) = delete;
		Instance(Instance&&);

		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&);


	private:
		VkInstance mInstance = {};
	};
}