cmake_minimum_required (VERSION 3.8)

if(OPENUI_ENABLE_SAMPLE)
add_executable (Sample
	Cairo/CairoGLPainter.cpp
	Cairo/CairoGLPainter.h
	Cairo/CairoGLRender.cpp
	Cairo/CairoGLRender.h
	Cairo/CairoUIPainter.cpp
	Cairo/CairoUIPainter.h 
	main.cpp 
	SDL3InputEnum.cpp
	SDL3InputEnum.h
)
add_dependencies(Sample OpenUI)

target_include_directories(Sample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(Sample PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/include)
target_include_directories(Sample PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/include/cairo)
target_include_directories(Sample PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/include/harfbuzz)
target_include_directories(Sample PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/include/glib-2.0)
target_include_directories(Sample PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/include/pango-1.0)
target_include_directories(Sample PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/lib/glib-2.0/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_directories(Sample PRIVATE ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/debug/lib)
else()
	target_link_directories(Sample PRIVATE ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/lib)
endif()

if(CMAKE_HOST_WIN32)
target_link_libraries(Sample PRIVATE Ws2_32 Dwrite)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(Sample PRIVATE GLEW::GLEW)
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(Sample PRIVATE SDL3::SDL3)
find_package(Stb REQUIRED)
target_include_directories(Sample PRIVATE ${Stb_INCLUDE_DIR})

target_link_libraries(Sample PRIVATE
		GlU32
		OpenGL32
		OpenUI
		brotlicommon
		brotlidec
		brotlienc
		debug bz2d optimized bz2
		cairo
		cairo-gobject
		cairo-script-interpreter
		charset
		ffi
		fontconfig
		debug freetyped optimized freetype
		fribidi
		getopt
		gio-2.0
		girepository-2.0
		glib-2.0
		GlU32
		gmodule-2.0
		gobject-2.0
		gthread-2.0
		harfbuzz
		harfbuzz-subset
		iconv
		intl
		debug libexpatdMD optimized libexpatMD
		debug libpng16d optimized libpng16
		OpenGL32
		pango-1.0
		pangocairo-1.0
		pangoft2-1.0
		pangowin32-1.0
		debug pcre2-8d optimized pcre2-8
		debug pcre2-16d optimized pcre2-16
		debug pcre2-32d optimized pcre2-32
		debug pcre2-posixd optimized pcre2-posix
		pixman-1
		debug pthreadVC3d optimized pthreadVC3
		debug pthreadVCE3d optimized pthreadVCE3
		debug pthreadVSE3d optimized pthreadVSE3
		debug zlibd optimized zlib
)

add_custom_command(  
    TARGET Sample POST_BUILD  
    COMMAND "${CMAKE_SOURCE_DIR}/INSTALL.bat"
    COMMENT "Copying dll to output directory"  
)
endif()