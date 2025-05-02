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
	public:
		static Core::Optional<Shader> Compile(Device& device, const std::filesystem::path& file);
		static Core::Optional<Shader> Compile(Device& device, const Core::IO::DynamicByteBuffer& bytes);


		Shader(const Shader& rhs)            = delete;
		Shader& operator=(const Shader& rhs) = delete;
		Shader(Shader&& rhs) noexcept;
		Shader& operator=(Shader&& rhs) noexcept;
		~Shader();


		operator VkShaderModule() const;


	protected:
		Shader(Device& device, VkShaderModule module);

	private:
		VkShaderModule                 mShaderModule;
		Core::ReflexivePointer<Device> mDevice;
	};
}
