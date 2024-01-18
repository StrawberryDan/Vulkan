//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Shader.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{

	Core::Optional<Shader> Shader::Compile(const Device& device, const std::filesystem::path& file)
	{
		if (auto bytes = Core::IO::DynamicByteBuffer::FromFile(file))
		{
			return Compile(device, bytes.Unwrap());
		}

		return Core::NullOpt;
	}


	Core::Optional<Shader> Shader::Compile(const Device& device, const Core::IO::DynamicByteBuffer& bytes)
	{
		VkShaderModuleCreateInfo createInfo {
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.codeSize = bytes.Size(),
				.pCode = reinterpret_cast<const uint32_t*>(bytes.Data()),
		};

		VkShaderModule shaderModule = VK_NULL_HANDLE;

		if (auto result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule); result == VK_SUCCESS)
		{
			return Shader(device, shaderModule);
		}
		else
		{
			return Core::NullOpt;
		}
	}


	Shader::Shader(Shader&& rhs) noexcept
		: mShaderModule(std::exchange(rhs.mShaderModule, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
	{

	}


	Shader& Shader::operator=(Shader&& rhs) noexcept
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	Shader::~Shader()
	{
		if (mShaderModule)
		{
			vkDestroyShaderModule(*mDevice, mShaderModule, nullptr);
		}
	}


	Shader::operator VkShaderModule_T*() const
	{
		return mShaderModule;
	}


	Shader::Shader(const Device& device, VkShaderModule module)
		: mShaderModule(module)
		, mDevice(device)
	{}
}
