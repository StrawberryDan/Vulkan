#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/AllocationError.hpp"
#include "Strawberry/Vulkan/Memory/AllocationRequest.hpp"
#include "Strawberry/Vulkan/Memory/Memory.hpp"
#include "Strawberry/Vulkan/Memory/MemoryPool.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Result.hpp"
// Vulkan
#include <vulkan/vulkan.h>


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


		virtual void Free(Allocation&& address) noexcept = 0;


		[[nodiscard]]       Device& GetDevice()       { return *mDevice; }
		[[nodiscard]] const Device& GetDevice() const { return *mDevice; }


	private:
		Core::ReflexivePointer<Device> mDevice;
	};






}
