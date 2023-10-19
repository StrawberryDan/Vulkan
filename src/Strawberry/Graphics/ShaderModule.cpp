//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ShaderModule.hpp"
#include "Device.cpp"
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>
#include <Strawberry/Core/IO/DynamicByteBuffer.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	ShaderModule::ShaderModule(const Device& device, const std::filesystem::path& spirvFile)
		: ShaderModule(device, Core::IO::DynamicByteBuffer::FromFile(spirvFile).Unwrap())
	{}


	ShaderModule::ShaderModule(const Device& device, const Core::IO::DynamicByteBuffer& bytes)
		: mDevice(device.mDevice)
	{
		VkShaderModuleCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = bytes.Size(),
			.pCode = reinterpret_cast<const uint32_t*>(bytes.Data()),
		};

		Core::AssertEQ(vkCreateShaderModule(mDevice, &createInfo, nullptr, &mShaderModule), VK_SUCCESS);
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
			vkDestroyShaderModule(mDevice, mShaderModule, nullptr);
		}
	}
}
