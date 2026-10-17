
if (OPENUI_ENABLE_OPENRT)

    add_definitions(-DOPENUI_ENABLE_OPENRT)
    add_definitions(-DOPENGL_IMPLEMENTATION)

    file(GLOB_RECURSE OPENRT_EXAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    add_executable(UI-OpenRT ${OPENRT_EXAMPLE_SRC})
    add_dependencies(UI-OpenRT OpenUI OpenRT nanovg Shader)

    target_link_libraries(UI-OpenRT PRIVATE OpenUI OpenRT nanovg)

    find_package(PkgConfig REQUIRED)

    find_package(Freetype REQUIRED)
    target_link_libraries(UI-OpenRT PRIVATE Freetype::Freetype) # since CMake 3.10

    find_package(Fontconfig REQUIRED) # since CMake 3.14
    target_link_libraries(UI-OpenRT PRIVATE Fontconfig::Fontconfig)

    find_package(harfbuzz CONFIG REQUIRED)
    target_link_libraries(UI-OpenRT PRIVATE harfbuzz::harfbuzz harfbuzz::harfbuzz-subset)

    pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)   # 不要 pangocairo
    target_link_libraries(UI-OpenRT PRIVATE PkgConfig::PANGO)
    pkg_check_modules(PANGOFT2 REQUIRED IMPORTED_TARGET pangoft2)   # 不要 pangocairo
    target_link_libraries(UI-OpenRT PRIVATE PkgConfig::PANGOFT2)

    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(UI-OpenRT PRIVATE SDL3::SDL3)

endif ()
