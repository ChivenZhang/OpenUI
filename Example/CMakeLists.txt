if(OPENUI_ENABLE_EXAMPLE)

	if(OPENUI_ENABLE_OPENRT)
		add_definitions(-DOPENUI_ENABLE_OPENRT)

		file(GLOB_RECURSE OPENRT_EXAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/OpenRT/*.cpp)
		add_executable (UI-OpenRT ${OPENRT_EXAMPLE_SRC})
		add_dependencies(UI-OpenRT OpenUI OpenRT NanoVG Shader)

		find_package(SDL3 CONFIG REQUIRED)
		target_link_libraries(UI-OpenRT PRIVATE SDL3::SDL3)

		target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
		target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo)
		target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
		target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
		target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
		target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_directories(UI-OpenRT PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
		else()
			target_link_directories(UI-OpenRT PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
		endif()

		target_link_libraries(UI-OpenRT PRIVATE OpenUI OpenRT NanoVG)
	endif ()

	if(OPENUI_ENABLE_SDLGPU)

		add_definitions(-DOPENUI_ENABLE_SDLGPU)

		if(CMAKE_HOST_WIN32)
			add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		endif()

		file(GLOB_RECURSE SDLGPU_EXAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/SDLGPU/*.cpp)
		add_executable (UI-SDLGPU ${SDLGPU_EXAMPLE_SRC})
		add_dependencies(UI-SDLGPU OpenUI OpenVG NanoVG Shader)

		find_package(SDL3 CONFIG REQUIRED)
		target_link_libraries(UI-SDLGPU PRIVATE SDL3::SDL3)

		target_include_directories(UI-SDLGPU PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
		target_include_directories(UI-SDLGPU PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo)
		target_include_directories(UI-SDLGPU PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
		target_include_directories(UI-SDLGPU PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
		target_include_directories(UI-SDLGPU PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
		target_include_directories(UI-SDLGPU PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_directories(UI-SDLGPU PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
		else()
			target_link_directories(UI-SDLGPU PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
		endif()

		target_link_libraries(UI-SDLGPU PRIVATE OpenUI OpenVG NanoVG)

	endif ()

endif()