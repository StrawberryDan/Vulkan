cmake_minimum_required(VERSION 3.26)
project(StrawberryGraphics)
include(FetchContent)


find_package(Vulkan REQUIRED)


FetchContent_Declare(GLFW
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.8")
FetchContent_MakeAvailable(GLFW)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif()


add_library(StrawberryGraphics
		src/Instance.cpp
		include/Strawberry/Graphics/Instance.hpp
		include/Strawberry/Graphics/Window.hpp
		src/Window.cpp)
target_link_libraries(StrawberryGraphics PUBLIC StrawberryCore glfw Vulkan::Vulkan)
target_include_directories(StrawberryGraphics PUBLIC include)
set_target_properties(StrawberryGraphics PROPERTIES CXX_STANDARD 20)


add_executable(StrawberryGraphics_Test test/Main.cpp)
target_link_libraries(StrawberryGraphics_Test PRIVATE StrawberryGraphics)
set_target_properties(StrawberryGraphics_Test PROPERTIES CXX_STANDARD 20)