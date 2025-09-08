#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/MemoryPool.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Allocator
		: public Core::EnableReflexivePointer
	{
	public:
		Allocator(Device& device);


		virtual void Free(MemoryBlock&& address) noexcept = 0;


		[[nodiscard]]       Device& GetDevice()       { return *mDevice; }
		[[nodiscard]] const Device& GetDevice() const { return *mDevice; }


	private:
		Core::ReflexivePointer<Device> mDevice;
	};






}
