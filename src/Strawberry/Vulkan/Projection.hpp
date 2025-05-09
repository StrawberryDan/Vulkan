#pragma once
#include "Strawberry/Core/Math/Matrix.hpp"


namespace Strawberry::Vulkan
{
	template<typename T>
	Core::Math::Matrix<T, 4, 4> Orthographic(float l, float r, float t, float b, float n, float f)
	{
		return Core::Math::Matrix<T, 4, 4>{
			2.0f / (r - l), 0.0f, 0.0f, (-2.0f * l) / (r - l) - 1.0f,
			0.0f, 2.0f / (t - b), 0.0f, (-2.0f * b) / (t - b) - 1.0f,
			0.0f, 0.0f, 1.0f / (f - n), -n / (f - n),
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}


	template<typename T>
	Core::Math::Matrix<T, 4, 4> Orthographic(float width, float height, float minDepth, float maxDepth)
	{
		return Orthographic<float>(-width / 2.0f, width / 2.0f, height / 2.0f, -height / 2.0f, minDepth, maxDepth);
	}


	template <typename T>
	Core::Math::Matrix<T, 4, 4> Perspective(float fov, float aspectRatio, float near, float far)
	{
		return Core::Math::Matrix<T, 4, 4>(
			1.0f / (aspectRatio * std::tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / (std::tanf(fov / 2)), 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f / (near - far), near / (near - far),
			0.0f, 0.0f, 1.0f, 0.0f
		);
	}
}