//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "SpriteSheet.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
#include "Strawberry/Graphics/Vulkan/Device.hpp"
// Strawberry Core
#include <Strawberry/Core/IO/DynamicByteBuffer.hpp>


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	Core::Optional<SpriteSheet>
	SpriteSheet::LoadFromFile(Vulkan::Device& device,
							  Vulkan::Queue& queue,
							  Core::Math::Vec2i spriteSize,
							  const std::filesystem::path& filepath)
	{
		// Read file
		auto fileContents = Core::IO::DynamicByteBuffer::FromImage(filepath);
		if (!fileContents) return Core::NullOpt;


		// Unpack file data
		auto [size, channels, bytes] = fileContents.Unwrap();
		Vulkan::Image image(device, size, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);



		// Return sprite sheet
		return SpriteSheet(queue, std::move(image), spriteSize);
	}


	SpriteSheet::SpriteSheet(const Vulkan::Queue& queue, Vulkan::Image image, Core::Math::Vec2i spriteSize)
		 : mDevice(queue.GetDevice())
		 , mQueue(queue)
		 , mCommandPool(mQueue->Create<Vulkan::CommandPool>(false))
		 , mImage(std::move(image))
		 , mSpriteSize(std::move(spriteSize))

	{}
}
