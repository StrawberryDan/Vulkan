//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ShaderModule.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Standard Library
#include <memory>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{

	Core::Optional<ShaderModule> ShaderModule::Compile(const Device& device, const std::filesystem::path& file)
	{
		if (auto bytes = Core::IO::DynamicByteBuffer::FromFile(file))
		{
			return Compile(device, bytes.Unwrap());
		}

		return Core::NullOpt;
	}


	Core::Optional<ShaderModule> ShaderModule::Compile(const Device& device, const Core::IO::DynamicByteBuffer& bytes)
	{
		VkShaderModuleCreateInfo createInfo {
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.codeSize = bytes.Size(),
				.pCode = reinterpret_cast<const uint32_t*>(bytes.Data()),
		};

		VkShaderModule shaderModule = VK_NULL_HANDLE;

		if (auto result = vkCreateShaderModule(device.mDevice, &createInfo, nullptr, &shaderModule); result == VK_SUCCESS)
		{
			return ShaderModule(device, shaderModule);
		}
		else
		{
			return Core::NullOpt;
		}
	}


	ShaderModule::ShaderModule(ShaderModule&& rhs) noexcept
		: mShaderModule(std::exchange(rhs.mShaderModule, nullptr))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
	{

	}


	ShaderModule& ShaderModule::operator=(ShaderModule&& rhs) noexcept
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	ShaderModule::~ShaderModule()
	{
		if (mShaderModule)
		{
			vkDestroyShaderModule(mDevice->mDevice, mShaderModule, nullptr);
		}
	}


	ShaderModule::ShaderModule(const Device& device, VkShaderModule module)
	{
	}
}
