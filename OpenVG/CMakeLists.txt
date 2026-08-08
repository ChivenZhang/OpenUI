cmake_minimum_required(VERSION 3.10...4.0)
project(OpenVG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(OPENVG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "OpenVG Source Path")

if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8)
endif()

file(GLOB_RECURSE OPENVG_SOURCE_FILES "*.cpp" "*.h")
add_executable(TestVG ${OPENVG_SOURCE_FILES})
target_include_directories(TestVG PUBLIC ./)

find_package(glm CONFIG REQUIRED)
target_link_libraries(TestVG PRIVATE glm::glm-header-only)
