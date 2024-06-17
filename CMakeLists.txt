cmake_minimum_required (VERSION 3.8)

project(rmgui-in-game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})  
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

#add_definitions( )

#add_library( )

add_subdirectory(Source)

add_executable (rmgui-in-game "rmgui-in-game.cpp" )

#add_dependencies(rmgui-in-game RMGUI)

target_link_libraries(rmgui-in-game RMGUI)