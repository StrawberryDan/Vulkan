//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "DeviceMemory.hpp"
#include "Device.hpp"
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <memory>
#include <cstring>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	DeviceMemory::DeviceMemory(const Device& device, uint32_t size, uint32_t typeMask, VkMemoryPropertyFlags properties)
		: mDevice(device)
		, mSize(size)
	{
		auto memoryTypeCandidates = device.GetPhysicalDevices()[0]->SearchMemoryTypes(typeMask, properties);
		Core::Assert(!memoryTypeCandidates.empty());


		VkMemoryAllocateInfo allocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = size,
			.memoryTypeIndex = memoryTypeCandidates[0]
		};

		Core::AssertEQ(vkAllocateMemory(mDevice, &allocateInfo, nullptr, &mDeviceMemory), VK_SUCCESS);
	}


	DeviceMemory::DeviceMemory(DeviceMemory&& rhs)
		: mDeviceMemory(std::exchange(rhs.mDeviceMemory, nullptr))
		, mSize(std::exchange(rhs.mSize, 0))
		, mDevice(std::exchange(rhs.mDevice, nullptr))
	{}


	DeviceMemory& DeviceMemory::operator=(DeviceMemory&& rhs)
	{
		if (this != &rhs)
		{
		    std::destroy_at(this);
		    std::construct_at(this, std::move(rhs));
		}

		return *this;
	}


	DeviceMemory::~DeviceMemory()
	{
		if (mDevice)
		{
			vkFreeMemory(mDevice, mDeviceMemory, nullptr);
		}
	}


	uint64_t DeviceMemory::GetSize() const
	{
		return mSize;
	}


	void DeviceMemory::SetData(const Core::IO::DynamicByteBuffer& bytes)
	{
		if (!mMappedDataPtr)
		{
			Core::AssertEQ(vkMapMemory(mDevice, mDeviceMemory, 0, GetSize(), 0, reinterpret_cast<void**>(&mMappedDataPtr)), VK_SUCCESS);
			Core::AssertNEQ(mMappedDataPtr, nullptr);
		}
		std::memcpy(mMappedDataPtr, bytes.Data(), bytes.Size());

		VkMappedMemoryRange range {
				.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				.pNext = nullptr,
				.memory = mDeviceMemory,
				.offset = 0,
				.size = GetSize(),
		};
		Core::AssertEQ(vkFlushMappedMemoryRanges(mDevice, 1, &range), VK_SUCCESS);
	}
}
