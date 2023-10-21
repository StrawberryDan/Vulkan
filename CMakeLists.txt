cmake_minimum_required(VERSION 3.26)
project(StrawberryGraphics)
include(FetchContent)


find_package(Vulkan REQUIRED)


add_custom_target(
		Shaders
		COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/data/Shaders/Mesh.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/data/Shaders/Mesh.vert.spirv
		COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/data/Shaders/SolidColor.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/data/Shaders/SolidColor.frag.spirv
)

FetchContent_Declare(GLFW
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.8")
FetchContent_MakeAvailable(GLFW)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif()


add_library(StrawberryGraphics
		src/Strawberry/Graphics/Instance.cpp
		src/Strawberry/Graphics/Instance.hpp
		src/Strawberry/Graphics/Window.hpp
		src/Strawberry/Graphics/Window.cpp
		src/Strawberry/Graphics/Event.hpp
		src/Strawberry/Graphics/Input/Key.hpp
		src/Strawberry/Graphics/Input/Key.cpp
		src/Strawberry/Graphics/Device.cpp
		src/Strawberry/Graphics/Device.hpp
		src/Strawberry/Graphics/Pipeline.cpp
		src/Strawberry/Graphics/Pipeline.hpp
		src/Strawberry/Graphics/Surface.cpp
		src/Strawberry/Graphics/Surface.hpp
		src/Strawberry/Graphics/Swapchain.cpp
		src/Strawberry/Graphics/Swapchain.hpp
		src/Strawberry/Graphics/Queue.cpp
		src/Strawberry/Graphics/Queue.hpp
		src/Strawberry/Graphics/CommandPool.cpp
		src/Strawberry/Graphics/CommandPool.hpp
		src/Strawberry/Graphics/CommandBuffer.cpp
		src/Strawberry/Graphics/CommandBuffer.hpp
		src/Strawberry/Graphics/Buffer.cpp
		src/Strawberry/Graphics/Buffer.hpp
		src/Strawberry/Graphics/BufferView.cpp
		src/Strawberry/Graphics/BufferView.hpp
		src/Strawberry/Graphics/DeviceMemory.cpp
		src/Strawberry/Graphics/DeviceMemory.hpp
		src/Strawberry/Graphics/Image.cpp
		src/Strawberry/Graphics/Image.hpp
		src/Strawberry/Graphics/ImageView.cpp
		src/Strawberry/Graphics/ImageView.hpp
		src/Strawberry/Graphics/ShaderModule.cpp
		src/Strawberry/Graphics/ShaderModule.hpp
		src/Strawberry/Graphics/Framebuffer.cpp
		src/Strawberry/Graphics/Framebuffer.hpp
)
target_link_libraries(StrawberryGraphics PUBLIC StrawberryCore glfw Vulkan::Vulkan)
target_include_directories(StrawberryGraphics PUBLIC src)
target_compile_definitions(StrawberryGraphics PUBLIC GLFW_INCLUDE_VULKAN)
set_target_properties(StrawberryGraphics PROPERTIES CXX_STANDARD 20)
add_dependencies(StrawberryGraphics Shaders)


add_executable(StrawberryGraphics_Test test/Main.cpp)
target_link_libraries(StrawberryGraphics_Test PRIVATE StrawberryGraphics)
set_target_properties(StrawberryGraphics_Test PROPERTIES CXX_STANDARD 20)
