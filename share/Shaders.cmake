find_package(Python3 REQUIRED)
find_package(Vulkan REQUIRED)


function(add_target_shader TARGET SOURCE DESTINATION)
	get_target_property(GRAPHICS_SOURCE_DIR StrawberryGraphics SOURCE_DIR)

	cmake_path(GET SOURCE FILENAME SHADER_FILENAME)
	set(SHADER_TARGET_NAME ${TARGET}-${SHADER_FILENAME})

	add_custom_command(OUTPUT ${DESTINATION}
			COMMAND ${Python3_EXECUTABLE} ${GRAPHICS_SOURCE_DIR}/share/compile_shader.py ${Vulkan_GLSLC_EXECUTABLE} ${SOURCE} ${DESTINATION}
			DEPENDS ${SOURCE}
	)

	target_sources(${TARGET} PUBLIC ${DESTINATION})
endfunction()
