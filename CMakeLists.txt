cmake_minimum_required (VERSION 3.8)

project(OpenUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})  
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

# Please copy x64-windows to External folder after install library below by vcpkg.
# vcpkg.exe install sdl2:x64-windows
# vcpkg.exe install glew:x64-windows
# vcpkg.exe install pango::x64-windows

include_directories(Source)
include_directories(External/x64-windows/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
link_directories(External/x64-windows/debug/lib)
else()
link_directories(External/x64-windows/lib)
endif()

add_subdirectory(External/Taitank)
add_subdirectory(Source)
add_subdirectory(Project)