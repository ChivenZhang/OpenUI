cmake_minimum_required (VERSION 3.8)

add_executable (OpenUI main.cpp )

add_dependencies(OpenUI RmGUI)

target_include_directories(OpenUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenUI)
