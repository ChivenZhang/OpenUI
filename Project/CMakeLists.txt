cmake_minimum_required (VERSION 3.8)

add_executable (OpenUI main.cpp  "SDL2Surface.h" "SDL2Surface.cpp")

add_dependencies(OpenUI RmGUI)

target_include_directories(OpenUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenUI 
	optimized SDL2 debug SDL2d
	optimized glew32 debug glew32d
	GlU32 
	OpenGL32 
	RmGUI
)
