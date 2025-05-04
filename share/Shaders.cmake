find_package(Python3 REQUIRED)
find_package(Vulkan REQUIRED)


function(add_target_shader TARGET SOURCE)
	get_target_property(GRAPHICS_SOURCE_DIR StrawberryVulkan SOURCE_DIR)

	cmake_path(GET SOURCE FILENAME SHADER_FILENAME)
	set(SHADER_TARGET_NAME ${TARGET}-${SHADER_FILENAME})


	file(CHMOD ${GRAPHICS_SOURCE_DIR}/share PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
	add_custom_command(OUTPUT ${SOURCE}.bin
			COMMAND ${Python3_EXECUTABLE} ${GRAPHICS_SOURCE_DIR}/share/compile_shader.py ${Vulkan_GLSLC_EXECUTABLE} ${SOURCE} ${SOURCE}.bin
			DEPENDS ${SOURCE} ${ARGN}
	)

	target_sources(${TARGET} PUBLIC ${SOURCE}.bin)
endfunction()


function(add_target_shaders)
	cmake_parse_arguments(
			ARG
			""
			"TARGET"
			"SHADERS;LIBRARIES"
			${ARGN}
	)


	foreach (SHADER ${ARG_SHADERS})
		add_target_shader(${ARG_TARGET} ${SHADER} ${ARG_LIBRARIES})
	endforeach ()
endfunction()
