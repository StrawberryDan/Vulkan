#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Graphics/Vulkan/Image.hpp"
#include "Strawberry/Graphics/Vulkan/CommandPool.hpp"
#include "Strawberry/Graphics/Vulkan/Queue.hpp"
#include "Strawberry/Graphics/Vulkan/CommandBuffer.hpp"
// Strawberry Core
#include <Strawberry/Core/Types/Optional.hpp>
#include <Strawberry/Core/Types/ReflexivePointer.hpp>
// Standard Libary
#include <filesystem>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Graphics
{
	class SpriteSheet
			: public Core::EnableReflexivePointer<SpriteSheet>
	{
	public:
		static Core::Optional<SpriteSheet>
		LoadFromFile(Vulkan::Device& device, Vulkan::Queue& queue, Core::Math::Vec2i spriteSize,
					 const std::filesystem::path& filepath);


	public:
		SpriteSheet(const Vulkan::Queue& queue, Vulkan::Image image, Core::Math::Vec2i spriteSize);


		template <std::movable T, typename... Args>
		T Create(Args... args) const { return T(*this, std::forward<Args>(args)...); }


	private:
		Core::ReflexivePointer<Vulkan::Device> mDevice;
		Core::ReflexivePointer<Vulkan::Queue> mQueue;
		Vulkan::CommandPool mCommandPool;


		Vulkan::Image mImage;
		Core::Math::Vec2i mSpriteSize;
	};
}
