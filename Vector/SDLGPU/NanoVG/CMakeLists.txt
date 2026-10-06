
file(GLOB_RECURSE NANOVG_SOURCE_FILES "*.cpp" "*.c" "*.h")
add_library(NanoVG STATIC ${NANOVG_SOURCE_FILES})

target_compile_definitions(NanoVG PUBLIC NANOVG_SDLGPU_IMPLEMENTATION)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(NanoVG PRIVATE SDL3::SDL3)