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

target_link_libraries(Sample 
	debug SDL3
	debug GlU32 
	debug OpenGL32 
	debug OpenUI
	debug brotlicommon
	debug brotlidec
	debug brotlienc
	debug bz2d
	debug cairo
	debug cairo-gobject
	debug cairo-script-interpreter
	debug charset
	debug ffi
	debug fontconfig
	debug freetyped
	debug fribidi
	debug getopt
	debug gio-2.0
	debug glew32d
	debug glib-2.0
	debug GlU32
	debug gmodule-2.0
	debug gobject-2.0
	debug gthread-2.0
	debug harfbuzz
	debug harfbuzz-subset
	debug iconv
	debug intl
	debug libexpatd
	debug libpng16d
	debug OpenGL32
	debug pango-1.0
	debug pangocairo-1.0
	debug pangoft2-1.0
	debug pangowin32-1.0
	debug pcre2-16d
	debug pcre2-32d
	debug pcre2-8d
	debug pcre2-posixd
	debug pixman-1
	debug pthreadVC3d
	debug pthreadVCE3d
	debug pthreadVSE3d
	debug zlibd
)

target_link_libraries(Sample 
	optimized SDL3
	optimized GlU32 
	optimized OpenGL32 
	optimized OpenUI
	optimized brotlicommon
	optimized brotlidec
	optimized brotlienc
	optimized bz2
	optimized cairo
	optimized cairo-gobject
	optimized cairo-script-interpreter
	optimized charset
	optimized ffi
	optimized fontconfig
	optimized freetype
	optimized fribidi
	optimized getopt
	optimized gio-2.0
	optimized glew32
	optimized glib-2.0
	optimized GlU32
	optimized gmodule-2.0
	optimized gobject-2.0
	optimized gthread-2.0
	optimized harfbuzz
	optimized harfbuzz-subset
	optimized iconv
	optimized intl
	optimized libexpat
	optimized libpng16
	optimized OpenGL32
	optimized pango-1.0
	optimized pangocairo-1.0
	optimized pangoft2-1.0
	optimized pangowin32-1.0
	optimized pcre2-16
	optimized pcre2-32
	optimized pcre2-8
	optimized pcre2-posix
	optimized pixman-1
	optimized pthreadVC3
	optimized pthreadVCE3
	optimized pthreadVSE3
	optimized zlib
)

add_custom_command(  
    TARGET Sample POST_BUILD  
    COMMAND "${CMAKE_SOURCE_DIR}/INSTALL.bat"
    COMMENT "Copying dll to output directory"  
)
endif()