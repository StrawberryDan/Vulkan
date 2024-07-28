#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Vulkan
#include "Strawberry/Vulkan/Memory/Allocator.hpp"
// Strawberry Core
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Standard Library
#include <concepts>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;


	class Buffer
	{
	public:
		Buffer(Allocator* allocator, const MemoryTypeCriteria& criteria, size_t size, VkBufferUsageFlags usage);
		Buffer(Allocator*                         allocator,
		       const MemoryTypeCriteria&          memoryCriteria,
		       const Core::IO::DynamicByteBuffer& bytes,
		       VkBufferUsageFlags                 usage);
		Buffer(const Buffer& rhs)            = delete;
		Buffer& operator=(const Buffer& rhs) = delete;
		Buffer(Buffer&& rhs) noexcept;
		Buffer& operator=(Buffer&& rhs) noexcept;
		~Buffer();


		operator VkBuffer() const;


		VkDevice GetDevice() const;


		template<std::movable T, typename... Args>
		T Create(const Args&... args) const
		{
			return T(*this, std::forward<const Args&>(args)...);
		}


		void SetData(const Core::IO::DynamicByteBuffer& bytes);


		[[nodiscard]] uint64_t GetSize() const;

	private:
		uint64_t       mSize;
		VkBuffer       mBuffer;
		AllocationView mMemory;
		VkDevice       mDevice;
	};
}
