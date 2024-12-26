cmake_minimum_required (VERSION 3.8)

if(OPENUI_ENABLE_SAMPLE)
add_executable (OpenUIDemo
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
add_dependencies(OpenUIDemo OpenUI)

find_package(GLEW REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE GLEW::GLEW)
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE SDL3::SDL3)
find_package(Stb REQUIRED)
target_include_directories(OpenUIDemo PRIVATE ${Stb_INCLUDE_DIR})

target_include_directories(OpenUIDemo PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
target_include_directories(OpenUIDemo PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo)
target_include_directories(OpenUIDemo PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
target_include_directories(OpenUIDemo PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
target_include_directories(OpenUIDemo PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
target_include_directories(OpenUIDemo PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_directories(OpenUIDemo PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
else()
	target_link_directories(OpenUIDemo PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
endif()

if(CMAKE_HOST_WIN32)
target_link_libraries(OpenUIDemo PRIVATE
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
		Ws2_32 Dwrite
)
add_custom_command(
		TARGET OpenUIDemo POST_BUILD
		COMMAND "${CMAKE_SOURCE_DIR}/INSTALL.bat"
		COMMENT "Copying dll to output directory"
)
endif()

if(CMAKE_HOST_APPLE)
target_link_libraries(OpenUIDemo PRIVATE
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
		gio-2.0
		girepository-2.0
		glib-2.0
		gmodule-2.0
		gobject-2.0
		gthread-2.0
		harfbuzz
		harfbuzz-subset
		iconv
		intl
		pango-1.0
		pangocairo-1.0
		pangoft2-1.0
		pixman-1
		expat
		debug png16d optimized png16
		pcre2-8
		pcre2-16
		pcre2-32
		pcre2-posix
		z
)
endif()

endif()