#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Standard Library
#include <concepts>
#include "DeviceMemory.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics::Vulkan
{
	class Device;


	class Buffer
	{
		friend class BufferView;
		friend class CommandBuffer;
		friend class Pipeline;
		friend class DescriptorSet;


	public:
		Buffer(const Device& device, uint64_t size, VkBufferUsageFlags usage);
		Buffer(const Device& device, const Core::IO::DynamicByteBuffer& bytes, VkBufferUsageFlags usage);
		Buffer(const Buffer& rhs) = delete;
		Buffer& operator=(const Buffer& rhs) = delete;
		Buffer(Buffer&& rhs) noexcept;
		Buffer& operator=(Buffer&& rhs) noexcept;
		~Buffer();


		template<std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


		void SetData(const Core::IO::DynamicByteBuffer& bytes);


		[[nodiscard]] uint64_t GetSize() const;


	private:
		uint64_t mSize;
		VkBuffer mBuffer;
		DeviceMemory mMemory;
		VkDevice mDevice;
		void* mMappedDataPtr = nullptr;
	};
}
