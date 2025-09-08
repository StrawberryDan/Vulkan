#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Resource/Buffer.hpp"
// Vulkan
#include <vulkan/vulkan.h>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;


	class BufferView
	{
	public:
		explicit BufferView(const Buffer& buffer, VkFormat format, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
		BufferView(const BufferView& rhs)            = delete;
		BufferView& operator=(const BufferView& rhs) = delete;
		BufferView(BufferView&& rhs) noexcept;
		BufferView& operator=(BufferView&& rhs) noexcept;
		~BufferView();

	private:
		VkBufferView mBufferView;
		VkDevice     mDevice;
	};
}
