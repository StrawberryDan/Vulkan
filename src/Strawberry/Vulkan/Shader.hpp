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
namespace Strawberry::Vulkan
{
	class Device;


	class Shader
	{
		friend class Pipeline;


	public:
		static Core::Optional<Shader> Compile(const Device& device, const std::filesystem::path& file);
		static Core::Optional<Shader> Compile(const Device& device, const Core::IO::DynamicByteBuffer& bytes);


		Shader(const Shader& rhs) = delete;
		Shader& operator=(const Shader& rhs) = delete;
		Shader(Shader&& rhs) noexcept;
		Shader& operator=(Shader&& rhs) noexcept;
		~Shader();


	protected:
		Shader(const Device& device, VkShaderModule module);


	private:
		VkShaderModule mShaderModule;
		Core::ReflexivePointer<Device> mDevice;
	};
}
