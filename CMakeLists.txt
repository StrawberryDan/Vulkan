cmake_minimum_required(VERSION 3.26)
project(StrawberryGraphics)



find_package(Vulkan REQUIRED)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif()


add_library(StrawberryGraphics
		src/Instance.cpp
		include/Strawberry/Graphics/Instance.hpp)
target_link_libraries(StrawberryGraphics PUBLIC StrawberryCore Vulkan::Vulkan)
target_include_directories(StrawberryGraphics PUBLIC include)
set_target_properties(StrawberryGraphics PROPERTIES CXX_STANDARD 20)


add_executable(StrawberryGraphics_Test test/Main.cpp)
target_link_libraries(StrawberryGraphics_Test PRIVATE StrawberryGraphics)
set_target_properties(StrawberryGraphics_Test PROPERTIES CXX_STANDARD 20)