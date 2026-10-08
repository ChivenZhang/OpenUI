
file(GLOB_RECURSE NANOVG_SOURCE_FILES "*.cpp" "*.c" "*.h")
add_library(NanoVG STATIC ${NANOVG_SOURCE_FILES})

target_compile_definitions(NanoVG PUBLIC NANOVG_SDLGPU_IMPLEMENTATION)

target_link_libraries(NanoVG PRIVATE OpenRT)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(NanoVG PRIVATE SDL3::SDL3)

find_package(Stb REQUIRED)
target_include_directories(NanoVG PRIVATE ${Stb_INCLUDE_DIR})