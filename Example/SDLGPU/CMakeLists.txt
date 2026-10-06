if(OPENUI_ENABLE_SDLGPU)

    add_definitions(-DOPENUI_ENABLE_SDLGPU)

    file(GLOB_RECURSE SDLGPU_EXAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
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