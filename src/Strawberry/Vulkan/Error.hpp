#pragma once
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Core/Types/Variant.hpp"


namespace Strawberry::Vulkan
{
	struct ErrorUnknown {};
	struct ErrorOutOfMemory {};

	using Error = Core::Variant<
		ErrorOutOfMemory,
		ErrorUnknown>;

	template <typename T>
	using Result = Core::Result<T, Error>;
}
