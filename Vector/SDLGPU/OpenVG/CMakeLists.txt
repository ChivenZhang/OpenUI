
file(GLOB_RECURSE OPENVG_SOURCE_FILES "*.cpp" "*.c" "*.h")
add_library(OpenVG STATIC ${OPENVG_SOURCE_FILES})

target_include_directories(OpenVG PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(OpenVG PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_directories(OpenVG PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
else()
	target_link_directories(OpenVG PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
endif()

target_link_libraries(OpenVG PRIVATE fribidi)

find_package(Freetype REQUIRED)
target_link_libraries(OpenVG PRIVATE Freetype::Freetype) # since CMake 3.10

find_package(Fontconfig REQUIRED) # since CMake 3.14
target_link_libraries(OpenVG PRIVATE Fontconfig::Fontconfig)

find_package(harfbuzz CONFIG REQUIRED)
# HarfBuzz font subsetter
target_link_libraries(OpenVG PRIVATE harfbuzz::harfbuzz-subset)
# HarfBuzz text shaping library
target_link_libraries(OpenVG PRIVATE harfbuzz::harfbuzz)