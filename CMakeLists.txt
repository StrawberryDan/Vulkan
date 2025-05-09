cmake_minimum_required(VERSION 3.26)
project(StrawberryVulkan)
include(FetchContent)


if (PROJECT_IS_TOP_LEVEL)
	FetchContent_Declare(StrawberryConfig
		GIT_REPOSITORY "https://github.com/StrawberryDan/Config.git"
		GIT_TAG "main")
	FetchContent_MakeAvailable(StrawberryConfig)
	include(StrawberryConfig)
endif ()


include(share/Shaders.cmake)


find_package(Vulkan REQUIRED)


if (NOT TARGET StrawberryVulkan)
	find_strawberry_library(NAMES Core Window)


	list(APPEND StrawberryVulkan_Source
		src/Strawberry/Vulkan/Buffer.cpp
		src/Strawberry/Vulkan/Buffer.hpp
		src/Strawberry/Vulkan/BufferView.cpp
		src/Strawberry/Vulkan/BufferView.hpp
		src/Strawberry/Vulkan/CommandBuffer.cpp
		src/Strawberry/Vulkan/CommandBuffer.hpp
		src/Strawberry/Vulkan/CommandPool.cpp
		src/Strawberry/Vulkan/CommandPool.hpp
		src/Strawberry/Vulkan/DescriptorPool.cpp
		src/Strawberry/Vulkan/DescriptorPool.hpp
		src/Strawberry/Vulkan/DescriptorSet.cpp
		src/Strawberry/Vulkan/DescriptorSet.hpp
		src/Strawberry/Vulkan/Device.cpp
		src/Strawberry/Vulkan/Device.hpp
		src/Strawberry/Vulkan/Fence.cpp
		src/Strawberry/Vulkan/Fence.hpp
		src/Strawberry/Vulkan/Framebuffer.cpp
		src/Strawberry/Vulkan/Framebuffer.hpp
		src/Strawberry/Vulkan/GraphicsPipeline.cpp
		src/Strawberry/Vulkan/GraphicsPipeline.hpp
		src/Strawberry/Vulkan/Image.cpp
		src/Strawberry/Vulkan/Image.hpp
		src/Strawberry/Vulkan/ImageMemoryBarrier.cpp
		src/Strawberry/Vulkan/ImageMemoryBarrier.hpp
		src/Strawberry/Vulkan/ImageView.cpp
		src/Strawberry/Vulkan/ImageView.hpp
		src/Strawberry/Vulkan/Instance.cpp
		src/Strawberry/Vulkan/Instance.hpp
		src/Strawberry/Vulkan/Memory/AllocationRequest.hpp
		src/Strawberry/Vulkan/Memory/Allocator.cpp
		src/Strawberry/Vulkan/Memory/Allocator.hpp
		src/Strawberry/Vulkan/Memory/FreelistAllocator.cpp
		src/Strawberry/Vulkan/Memory/FreelistAllocator.hpp
		src/Strawberry/Vulkan/Memory/Memory.cpp
		src/Strawberry/Vulkan/Memory/Memory.hpp
		src/Strawberry/Vulkan/Memory/MemoryTypeCriteria.cpp
		src/Strawberry/Vulkan/Memory/MemoryTypeCriteria.hpp
		src/Strawberry/Vulkan/Memory/NaiveAllocator.cpp
		src/Strawberry/Vulkan/Memory/NaiveAllocator.hpp
		src/Strawberry/Vulkan/PhysicalDevice.cpp
		src/Strawberry/Vulkan/PhysicalDevice.hpp
		src/Strawberry/Vulkan/PipelineLayout.cpp
		src/Strawberry/Vulkan/PipelineLayout.hpp
		src/Strawberry/Vulkan/Projection.hpp
		src/Strawberry/Vulkan/Queue.cpp
		src/Strawberry/Vulkan/Queue.hpp
		src/Strawberry/Vulkan/RenderPass.cpp
		src/Strawberry/Vulkan/RenderPass.hpp
		src/Strawberry/Vulkan/Sampler.cpp
		src/Strawberry/Vulkan/Sampler.hpp
		src/Strawberry/Vulkan/Shader.cpp
		src/Strawberry/Vulkan/Shader.hpp
		src/Strawberry/Vulkan/Surface.cpp
		src/Strawberry/Vulkan/Surface.hpp
		src/Strawberry/Vulkan/Swapchain.cpp
		src/Strawberry/Vulkan/Swapchain.hpp
	)


	new_strawberry_library(NAME StrawberryVulkan SOURCE ${StrawberryVulkan_Source})


	target_link_libraries(StrawberryVulkan PUBLIC StrawberryCore StrawberryWindow Vulkan::Vulkan)
	target_include_directories(StrawberryVulkan PUBLIC src)
	target_compile_definitions(StrawberryVulkan PUBLIC GLFW_INCLUDE_VULKAN)
	set_target_properties(StrawberryVulkan PROPERTIES CXX_STANDARD 23)


	add_executable(StrawberryVulkanTest test/Main.cpp)
	target_link_libraries(StrawberryVulkanTest PRIVATE StrawberryVulkan)
	add_target_shaders(TARGET StrawberryVulkanTest SHADERS
		${CMAKE_CURRENT_SOURCE_DIR}/test/Mesh.vert
		${CMAKE_CURRENT_SOURCE_DIR}/test/SolidColor.frag
		${CMAKE_CURRENT_SOURCE_DIR}/test/Texture.frag)
endif()