#pragma once


namespace Strawberry::Vulkan
{
	class Allocation;


	// Class representing a block of allocated Vulkan Memory.
	class MemoryPool final
			: public Core::EnableReflexivePointer
	{
	public:
		static Core::Result<MemoryPool, AllocationError> Allocate(Device& device, MemoryTypeIndex memoryTypeIndex, size_t size);


		MemoryPool() = default;
		MemoryPool(Device& device, MemoryTypeIndex memoryTypeIndex, VkDeviceMemory memory, size_t size);
		MemoryPool(const MemoryPool&)            = delete;
		MemoryPool& operator=(const MemoryPool&) = delete;
		MemoryPool(MemoryPool&& other) noexcept;
		MemoryPool& operator=(MemoryPool&& other) noexcept;
		~MemoryPool() override;


		Allocation AllocateView(Allocator& allocator, size_t offset, size_t size);


		Core::ReflexivePointer<Device> GetDevice() const noexcept;
		VkDeviceMemory Memory() const noexcept;
		MemoryTypeIndex GetMemoryTypeIndex() const noexcept;
		size_t Size() const noexcept;


		VkMemoryPropertyFlags Properties() const;
		uint8_t*              GetMappedAddress() const noexcept;


		void Flush() const noexcept;
		void Overwrite(const Core::IO::DynamicByteBuffer& bytes) const noexcept;

	private:
		Core::ReflexivePointer<Device>         mDevice          = nullptr;
		MemoryTypeIndex                        mMemoryTypeIndex = {};
		VkDeviceMemory                         mMemory          = VK_NULL_HANDLE;
		size_t                                 mSize            = 0;
		mutable Core::Optional<uint8_t*>       mMappedAddress   = Core::NullOpt;
	};
}