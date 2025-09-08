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
            src/Strawberry/Vulkan/Resource/Buffer.cpp
            src/Strawberry/Vulkan/Resource/Buffer.hpp
            src/Strawberry/Vulkan/Resource/BufferView.cpp
            src/Strawberry/Vulkan/Resource/BufferView.hpp
            src/Strawberry/Vulkan/Queue/CommandBuffer.cpp
            src/Strawberry/Vulkan/Queue/CommandBuffer.hpp
            src/Strawberry/Vulkan/Queue/CommandPool.cpp
            src/Strawberry/Vulkan/Queue/CommandPool.hpp
            src/Strawberry/Vulkan/Pipeline/ComputePipeline.cpp
            src/Strawberry/Vulkan/Pipeline/ComputePipeline.hpp
            src/Strawberry/Vulkan/Descriptor/DescriptorPool.cpp
            src/Strawberry/Vulkan/Descriptor/DescriptorPool.hpp
            src/Strawberry/Vulkan/Descriptor/DescriptorSet.cpp
            src/Strawberry/Vulkan/Descriptor/DescriptorSet.hpp
            src/Strawberry/Vulkan/Device/Device.cpp
            src/Strawberry/Vulkan/Device/Device.hpp
            src/Strawberry/Vulkan/Synchronisation/Fence.cpp
            src/Strawberry/Vulkan/Synchronisation/Fence.hpp
            src/Strawberry/Vulkan/Resource/Framebuffer.cpp
            src/Strawberry/Vulkan/Resource/Framebuffer.hpp
            src/Strawberry/Vulkan/Pipeline/GraphicsPipeline.cpp
            src/Strawberry/Vulkan/Pipeline/GraphicsPipeline.hpp
            src/Strawberry/Vulkan/Resource/Image.cpp
            src/Strawberry/Vulkan/Resource/Image.hpp
            src/Strawberry/Vulkan/Queue/ImageMemoryBarrier.cpp
            src/Strawberry/Vulkan/Queue/ImageMemoryBarrier.hpp
            src/Strawberry/Vulkan/Resource/ImageView.cpp
            src/Strawberry/Vulkan/Resource/ImageView.hpp
            src/Strawberry/Vulkan/Device/Instance.cpp
            src/Strawberry/Vulkan/Device/Instance.hpp
            src/Strawberry/Vulkan/Memory/MemoryBlock.cpp
            src/Strawberry/Vulkan/Memory/MemoryBlock.hpp
            src/Strawberry/Vulkan/Memory/Allocator/AllocationError.hpp
            src/Strawberry/Vulkan/Memory/Allocator/AllocationRequest.hpp
            src/Strawberry/Vulkan/Memory/Allocator/Allocator.cpp
            src/Strawberry/Vulkan/Memory/Allocator/Allocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/ChainAllocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/FallbackAllocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/FreelistAllocator.cpp
            src/Strawberry/Vulkan/Memory/Allocator/FreelistAllocator.hpp
		src/Strawberry/Vulkan/Memory/Memory.cpp
		src/Strawberry/Vulkan/Memory/Memory.hpp
		src/Strawberry/Vulkan/Memory/MemoryPool.cpp
		src/Strawberry/Vulkan/Memory/MemoryPool.hpp
		src/Strawberry/Vulkan/Memory/MemoryTypeCriteria.cpp
		src/Strawberry/Vulkan/Memory/MemoryTypeCriteria.hpp
            src/Strawberry/Vulkan/Memory/Allocator/MultiAllocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/NaiveAllocator.cpp
            src/Strawberry/Vulkan/Memory/Allocator/NaiveAllocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/NaiveMultiAllocator.cpp
            src/Strawberry/Vulkan/Memory/Allocator/NaiveMultiAllocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/PoolAllocator.cpp
            src/Strawberry/Vulkan/Memory/Allocator/PoolAllocator.hpp
            src/Strawberry/Vulkan/Memory/Allocator/SingleAllocator.cpp
            src/Strawberry/Vulkan/Memory/Allocator/SingleAllocator.hpp
            src/Strawberry/Vulkan/Device/PhysicalDevice.cpp
            src/Strawberry/Vulkan/Device/PhysicalDevice.hpp
            src/Strawberry/Vulkan/Pipeline/PipelineLayout.cpp
            src/Strawberry/Vulkan/Pipeline/PipelineLayout.hpp
            src/Strawberry/Vulkan/Math/Projection.hpp
            src/Strawberry/Vulkan/Queue/Queue.cpp
            src/Strawberry/Vulkan/Queue/Queue.hpp
            src/Strawberry/Vulkan/Pipeline/RenderPass.cpp
            src/Strawberry/Vulkan/Pipeline/RenderPass.hpp
            src/Strawberry/Vulkan/Descriptor/Sampler.cpp
            src/Strawberry/Vulkan/Descriptor/Sampler.hpp
            src/Strawberry/Vulkan/Pipeline/Shader.cpp
            src/Strawberry/Vulkan/Pipeline/Shader.hpp
            src/Strawberry/Vulkan/Device/Surface.cpp
            src/Strawberry/Vulkan/Device/Surface.hpp
            src/Strawberry/Vulkan/Device/Swapchain.cpp
            src/Strawberry/Vulkan/Device/Swapchain.hpp
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
		${CMAKE_CURRENT_SOURCE_DIR}/test/Pattern.comp
		${CMAKE_CURRENT_SOURCE_DIR}/test/Texture.frag)
endif()