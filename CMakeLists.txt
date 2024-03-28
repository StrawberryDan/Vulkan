cmake_minimum_required(VERSION 3.26)
project(StrawberryVulkan)
include(FetchContent)


if (PROJECT_IS_TOP_LEVEL)
	FetchContent_Declare(StrawberryConfig
			GIT_REPOSITORY "https://github.com/StrawberryDan/Config.git"
			GIT_TAG "main")
	FetchContent_MakeAvailable(StrawberryConfig)
	include(StrawberryConfig)
endif()


include(share/Shaders.cmake)


find_package(Vulkan REQUIRED)


option(FT_DISABLE_BROTLI "" ON)
option(FT_DISABLE_BZIP2 "" ON)
option(FT_DISABLE_HARFBUZZ "" ON)
option(FT_DISABLE_PNG "" ON)
option(FT_DISABLE_ZLIB "" ON)
FetchContent_Declare(FREETYPE
	GIT_REPOSITORY "https://gitlab.freedesktop.org/freetype/freetype.git"
)
FetchContent_MakeAvailable(FREETYPE)


if (NOT TARGET StrawberryCore AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif()


if (NOT TARGET StrawberryWindow AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Window)
	add_subdirectory(../Window ${CMAKE_CURRENT_BINARY_DIR}/Window)
endif()


add_library(StrawberryVulkan STATIC
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
		src/Strawberry/Vulkan/DeviceMemory.cpp
		src/Strawberry/Vulkan/DeviceMemory.hpp
		src/Strawberry/Vulkan/Fence.cpp
		src/Strawberry/Vulkan/Fence.hpp
		src/Strawberry/Vulkan/Framebuffer.cpp
		src/Strawberry/Vulkan/Framebuffer.hpp
		src/Strawberry/Vulkan/GraphicsPipeline.cpp
		src/Strawberry/Vulkan/GraphicsPipeline.hpp
		src/Strawberry/Vulkan/Image.cpp
		src/Strawberry/Vulkan/Image.hpp
		src/Strawberry/Vulkan/ImageMemoryBarrier.hpp
		src/Strawberry/Vulkan/ImageMemoryBarrier.cpp
		src/Strawberry/Vulkan/ImageView.cpp
		src/Strawberry/Vulkan/ImageView.hpp
		src/Strawberry/Vulkan/Instance.cpp
		src/Strawberry/Vulkan/Instance.hpp
		src/Strawberry/Vulkan/PhysicalDevice.cpp
		src/Strawberry/Vulkan/PhysicalDevice.hpp
		src/Strawberry/Vulkan/PipelineLayout.cpp
		src/Strawberry/Vulkan/PipelineLayout.hpp
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
target_link_libraries(StrawberryVulkan PUBLIC StrawberryCore StrawberryWindow Vulkan::Vulkan freetype)
target_include_directories(StrawberryVulkan PUBLIC src)
target_compile_definitions(StrawberryVulkan PUBLIC GLFW_INCLUDE_VULKAN)
set_target_properties(StrawberryVulkan PROPERTIES CXX_STANDARD 20)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_link_options(StrawberryVulkan PRIVATE "-static")
endif()


add_executable(StrawberryVulkanTest test/Main.cpp)
target_link_libraries(StrawberryVulkanTest PRIVATE StrawberryVulkan)
set_target_properties(StrawberryVulkanTest PROPERTIES CXX_STANDARD 20)
add_target_shaders(TARGET StrawberryVulkanTest SHADERS
		${CMAKE_CURRENT_SOURCE_DIR}/test/Mesh.vert
		${CMAKE_CURRENT_SOURCE_DIR}/test/SolidColor.frag
		${CMAKE_CURRENT_SOURCE_DIR}/test/Texture.frag)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_link_options(StrawberryVulkanTest PRIVATE "-static")
endif()
