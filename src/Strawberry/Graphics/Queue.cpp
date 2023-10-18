//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Queue.hpp"
#include "Device.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Queue::Queue(const Device& device)
	{
		vkGetDeviceQueue(device.mDevice, device.mQueueFamilyIndex, 0, &mQueue);
	}


	Queue::Queue(Queue&& rhs)
		: mQueue(std::exchange(rhs.mQueue, nullptr))
	{}


	Queue& Queue::operator=(Queue&& rhs)
	{
		if (this != &rhs)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(rhs));
		}

		return *this;
	}
}
