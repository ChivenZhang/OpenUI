
if (OPENUI_ENABLE_OPENRT)

    add_definitions(-DOPENUI_ENABLE_OPENRT)
    add_definitions(-DOPENGL_IMPLEMENTATION)

    file(GLOB_RECURSE OPENRT_EXAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    add_executable(UI-OpenRT ${OPENRT_EXAMPLE_SRC})
    add_dependencies(UI-OpenRT OpenUI OpenRT NanoVG Shader)

    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(UI-OpenRT PRIVATE SDL3::SDL3)

    target_link_libraries(UI-OpenRT PRIVATE OpenUI OpenRT NanoVG)

    target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
    target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
    target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
    target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
    target_include_directories(UI-OpenRT PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_directories(UI-OpenRT PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
    else ()
        target_link_directories(UI-OpenRT PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
    endif ()

endif ()
