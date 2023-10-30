cmake_minimum_required(VERSION 3.26)
project(StrawberryGraphics)
include(FetchContent)


include(share/Shaders.cmake)


find_package(Vulkan REQUIRED)


FetchContent_Declare(GLFW
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.8")
FetchContent_MakeAvailable(GLFW)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif()


add_library(StrawberryGraphics
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
		src/Strawberry/Graphics/Vulkan/ShaderModule.cpp
		src/Strawberry/Graphics/Vulkan/ShaderModule.hpp
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
)
target_link_libraries(StrawberryGraphics PUBLIC StrawberryCore glfw Vulkan::Vulkan)
target_include_directories(StrawberryGraphics PUBLIC src)
target_compile_definitions(StrawberryGraphics PUBLIC GLFW_INCLUDE_VULKAN)
set_target_properties(StrawberryGraphics PROPERTIES CXX_STANDARD 20)
add_target_shader(StrawberryGraphics src/Strawberry/Graphics/2D/Sprite.vert src/Strawberry/Graphics/2D/Sprite.vert.bin)
add_target_shader(StrawberryGraphics src/Strawberry/Graphics/2D/Sprite.frag src/Strawberry/Graphics/2D/Sprite.frag.bin)


add_executable(StrawberryGraphics_Test test/Main.cpp)
target_link_libraries(StrawberryGraphics_Test PRIVATE StrawberryGraphics)
set_target_properties(StrawberryGraphics_Test PROPERTIES CXX_STANDARD 20)
add_target_shader(StrawberryGraphics_Test test/Mesh.vert test/Mesh.vert.bin)
add_target_shader(StrawberryGraphics_Test test/SolidColor.frag test/SolidColor.frag.bin)
add_target_shader(StrawberryGraphics_Test test/Texture.frag test/Texture.frag.bin)
