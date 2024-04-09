#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/Variant.hpp"
#include "Strawberry/Core/Types/Result.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	struct GPUAddress
	{
		VkDeviceMemory deviceMemory;
		size_t         offset;
	};


	struct GPUMemoryRange
	{
		GPUAddress address;
		size_t     size;
	};


	struct AllocatorError
	{

	};


	template <typename T>
	concept Allocator = requires(T allocator, GPUAddress gpuAddress)
	{
		{ allocator.Allocate() } -> std::same_as<Core::Result<GPUAddress, AllocatorError>>;
		{ allocator.Free(gpuAddress) };
	};
}