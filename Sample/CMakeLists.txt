if(OPENUI_ENABLE_SAMPLE)

	if(OPENUI_ENABLE_OPENRT)
		add_definitions(-DOPENUI_ENABLE_OpenRT)

		file(GLOB_RECURSE OPENRT_SAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/OpenRT/*.cpp)
		add_executable (OpenRT-Sample ${OPENRT_SAMPLE_SRC})
		add_dependencies(OpenRT-Sample OpenUI OpenRT NanoVG Shader)

		find_package(SDL3 CONFIG REQUIRED)
		target_link_libraries(OpenRT-Sample PRIVATE SDL3::SDL3)

		target_include_directories(OpenRT-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
		target_include_directories(OpenRT-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo)
		target_include_directories(OpenRT-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
		target_include_directories(OpenRT-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
		target_include_directories(OpenRT-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
		target_include_directories(OpenRT-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_directories(OpenRT-Sample PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
		else()
			target_link_directories(OpenRT-Sample PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
		endif()

		target_link_libraries(OpenRT-Sample PRIVATE OpenUI OpenRT NanoVG)
	endif ()

	if(OPENUI_ENABLE_SDLGPU)

		add_definitions(-DOPENUI_ENABLE_SDLGPU)

		if(CMAKE_HOST_WIN32)
			add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		endif()

		file(GLOB_RECURSE SDLGPU_SAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/SDLGPU/*.cpp)
		add_executable (SDLGPU-Sample ${SDLGPU_SAMPLE_SRC})
		add_dependencies(SDLGPU-Sample OpenUI OpenVG NanoVG Shader)

		find_package(SDL3 CONFIG REQUIRED)
		target_link_libraries(SDLGPU-Sample PRIVATE SDL3::SDL3)

		target_include_directories(SDLGPU-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
		target_include_directories(SDLGPU-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo)
		target_include_directories(SDLGPU-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
		target_include_directories(SDLGPU-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
		target_include_directories(SDLGPU-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
		target_include_directories(SDLGPU-Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_directories(SDLGPU-Sample PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
		else()
			target_link_directories(SDLGPU-Sample PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
		endif()

		target_link_libraries(SDLGPU-Sample PRIVATE OpenUI OpenVG NanoVG)

	endif ()

endif()