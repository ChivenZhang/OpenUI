if(OPENUI_ENABLE_EXAMPLE)

	if(CMAKE_HOST_WIN32)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	endif()

	add_subdirectory(OpenRT)

	add_subdirectory(OpenVG)

	add_subdirectory(SDLGPU)

endif()