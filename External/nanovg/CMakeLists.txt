
file(GLOB_RECURSE NANOVG_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_library(nanovg STATIC ${NANOVG_SOURCE_FILES})

target_link_libraries(nanovg PRIVATE OpenRT)

find_package(Stb REQUIRED)
target_include_directories(nanovg PRIVATE ${Stb_INCLUDE_DIR})