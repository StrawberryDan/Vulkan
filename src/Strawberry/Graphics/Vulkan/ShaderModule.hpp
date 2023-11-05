#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
#include <Strawberry/Core/IO/DynamicByteBuffer.hpp>
// Standard Library
#include <filesystem>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;


	class ShaderModule
	{
		friend class Pipeline;


	public:
		static Core::Optional<ShaderModule> Compile(const Device& device, const std::filesystem::path& file);
		static Core::Optional<ShaderModule> Compile(const Device& device, const Core::IO::DynamicByteBuffer& bytes);


		ShaderModule(const ShaderModule& rhs) = delete;
		ShaderModule& operator=(const ShaderModule& rhs) = delete;
		ShaderModule(ShaderModule&& rhs) noexcept;
		ShaderModule& operator=(ShaderModule&& rhs) noexcept;
		~ShaderModule();


	protected:
		ShaderModule(const Device& device, VkShaderModule module);


	private:
		VkShaderModule mShaderModule;
		Core::ReflexivePointer<Device> mDevice;
	};
}
