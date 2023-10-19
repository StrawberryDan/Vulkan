#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
#include <Strawberry/Core/IO/DynamicByteBuffer.hpp>
// Standard Library
#include <filesystem>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class Device;


	class ShaderModule
	{
	public:
		ShaderModule(const Device& device, const std::filesystem::path& spirvFile);
		ShaderModule(const Device& device, const Core::IO::DynamicByteBuffer& bytes);
		ShaderModule(const ShaderModule& rhs) = delete;
		ShaderModule& operator=(const ShaderModule& rhs) = delete;
		ShaderModule(ShaderModule&& rhs) noexcept;
		ShaderModule& operator=(ShaderModule&& rhs) noexcept;
		~ShaderModule();


	private:
		VkShaderModule mShaderModule;
		VkDevice mDevice;
	};
}
