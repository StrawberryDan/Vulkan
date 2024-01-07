cmake_minimum_required(VERSION 3.26)
project(StrawberryGraphics)
include(FetchContent)


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


FetchContent_Declare(GLFW
	EXCLUDE_FROM_ALL
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.8")
FetchContent_MakeAvailable(GLFW)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif()


add_library(StrawberryGraphics STATIC
		src/Strawberry/Graphics/Vulkan/Instance.cpp
		src/Strawberry/Graphics/Vulkan/Instance.hpp
		src/Strawberry/Graphics/Window.hpp
		src/Strawberry/Graphics/Window.cpp
		src/Strawberry/Graphics/Event.hpp
		src/Strawberry/Graphics/Input/Key.hpp
		src/Strawberry/Graphics/Input/Key.cpp
		src/Strawberry/Graphics/Vulkan/Device.cpp
		src/Strawberry/Graphics/Vulkan/Device.hpp
		src/Strawberry/Graphics/Vulkan/Pipeline.cpp
		src/Strawberry/Graphics/Vulkan/Pipeline.hpp
		src/Strawberry/Graphics/Vulkan/Surface.cpp
		src/Strawberry/Graphics/Vulkan/Surface.hpp
		src/Strawberry/Graphics/Vulkan/Swapchain.cpp
		src/Strawberry/Graphics/Vulkan/Swapchain.hpp
		src/Strawberry/Graphics/Vulkan/Queue.cpp
		src/Strawberry/Graphics/Vulkan/Queue.hpp
		src/Strawberry/Graphics/Vulkan/CommandPool.cpp
		src/Strawberry/Graphics/Vulkan/CommandPool.hpp
		src/Strawberry/Graphics/Vulkan/CommandBuffer.cpp
		src/Strawberry/Graphics/Vulkan/CommandBuffer.hpp
		src/Strawberry/Graphics/Vulkan/Buffer.cpp
		src/Strawberry/Graphics/Vulkan/Buffer.hpp
		src/Strawberry/Graphics/Vulkan/BufferView.cpp
		src/Strawberry/Graphics/Vulkan/BufferView.hpp
		src/Strawberry/Graphics/Vulkan/DeviceMemory.cpp
		src/Strawberry/Graphics/Vulkan/DeviceMemory.hpp
		src/Strawberry/Graphics/Vulkan/Image.cpp
		src/Strawberry/Graphics/Vulkan/Image.hpp
		src/Strawberry/Graphics/Vulkan/ImageView.cpp
		src/Strawberry/Graphics/Vulkan/ImageView.hpp
        src/Strawberry/Graphics/Vulkan/Shader.cpp
        src/Strawberry/Graphics/Vulkan/Shader.hpp
		src/Strawberry/Graphics/Vulkan/Framebuffer.cpp
		src/Strawberry/Graphics/Vulkan/Framebuffer.hpp
		src/Strawberry/Graphics/Vulkan/Fence.cpp
		src/Strawberry/Graphics/Vulkan/Fence.hpp
		src/Strawberry/Graphics/Vulkan/Sampler.cpp
		src/Strawberry/Graphics/Vulkan/Sampler.hpp
		src/Strawberry/Graphics/Vulkan/RenderPass.cpp
		src/Strawberry/Graphics/Vulkan/RenderPass.hpp
		src/Strawberry/Graphics/2D/Sprite.cpp
		src/Strawberry/Graphics/2D/Sprite.hpp
		src/Strawberry/Graphics/2D/Transform2D.cpp
		src/Strawberry/Graphics/2D/Transform2D.hpp
		src/Strawberry/Graphics/2D/SpriteSheet.cpp
		src/Strawberry/Graphics/2D/SpriteSheet.hpp
		src/Strawberry/Graphics/2D/SpriteRenderer.cpp
		src/Strawberry/Graphics/2D/SpriteRenderer.hpp
		src/Strawberry/Graphics/Vulkan/DescriptorSet.cpp
		src/Strawberry/Graphics/Vulkan/DescriptorSet.hpp
		src/Strawberry/Graphics/Vulkan/DescriptorPool.cpp
		src/Strawberry/Graphics/Vulkan/DescriptorPool.hpp
		src/Strawberry/Graphics/2D/FontFace.cpp
		src/Strawberry/Graphics/2D/FontFace.hpp
		src/Strawberry/Graphics/2D/TextRenderer.cpp
		src/Strawberry/Graphics/2D/TextRenderer.hpp
        src/Strawberry/Graphics/Renderer.hpp
		src/Strawberry/Graphics/Renderer.cpp
		src/Strawberry/Graphics/2D/ImageRenderer.hpp
		src/Strawberry/Graphics/2D/ImageRenderer.cpp
		src/Strawberry/Graphics/Input/Mouse.hpp
		src/Strawberry/Graphics/Vulkan/PhysicalDevice.cpp
		src/Strawberry/Graphics/Vulkan/PhysicalDevice.hpp
)
target_link_libraries(StrawberryGraphics PUBLIC StrawberryCore glfw Vulkan::Vulkan freetype)
target_include_directories(StrawberryGraphics PUBLIC src)
target_compile_definitions(StrawberryGraphics PUBLIC GLFW_INCLUDE_VULKAN)
set_target_properties(StrawberryGraphics PROPERTIES CXX_STANDARD 20)
add_target_shader(StrawberryGraphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Strawberry/Graphics/2D/Sprite.vert)
add_target_shader(StrawberryGraphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Strawberry/Graphics/2D/Sprite.frag)
add_target_shader(StrawberryGraphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Strawberry/Graphics/2D/Text.vert)
add_target_shader(StrawberryGraphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Strawberry/Graphics/2D/Text.frag)
add_target_shader(StrawberryGraphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Strawberry/Graphics/2D/Image.vert)
add_target_shader(StrawberryGraphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Strawberry/Graphics/2D/Image.frag)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_link_options(StrawberryGraphics PRIVATE "-static")
endif()


add_executable(StrawberryGraphics_Test test/Main.cpp)
target_link_libraries(StrawberryGraphics_Test PRIVATE StrawberryGraphics)
set_target_properties(StrawberryGraphics_Test PROPERTIES CXX_STANDARD 20)
add_target_shader(StrawberryGraphics_Test ${CMAKE_CURRENT_SOURCE_DIR}/test/Mesh.vert)
add_target_shader(StrawberryGraphics_Test ${CMAKE_CURRENT_SOURCE_DIR}/test/SolidColor.frag)
add_target_shader(StrawberryGraphics_Test ${CMAKE_CURRENT_SOURCE_DIR}/test/Texture.frag)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_link_options(StrawberryGraphics_Test PRIVATE "-static")
endif()
