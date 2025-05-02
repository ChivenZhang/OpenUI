if(OPENUI_ENABLE_SAMPLE)

if(OPENUI_ENABLE_OPENGL)
	add_definitions(-DOPENUI_ENABLE_OPENGL)
endif ()
if(OPENUI_ENABLE_VULKAN)
	add_definitions(-DOPENUI_ENABLE_VULKAN)
endif ()
if(OPENUI_ENABLE_DIRECTX)
	add_definitions(-DOPENUI_ENABLE_DIRECTX -DNOMINMAX)
endif ()
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable (OpenUIDemo
		main.cpp
		SDL3InputEnum.cpp
		SDL3InputEnum.h
		Cairo/CairoUIPainter.cpp
		Cairo/CairoUIPainter.h
		Metal/CairoMTRender.cpp
		Metal/CairoMTRender.h
		Metal/SDL3MTDevice.h
		DirectX/CairoDXRender.cpp
		DirectX/CairoDXRender.h
		DirectX/SDL3DXDevice.h
		OpenGL/CairoGLRender.cpp
		OpenGL/CairoGLRender.h
		OpenGL/SDL3GLDevice.h
		Vulkan/CairoVKRender.cpp
		Vulkan/CairoVKRender.h
		Vulkan/SDL3VKDevice.h
		WebGL/CairoWGLRender.cpp
		WebGL/CairoWGLRender.h
		WebGL/WebGLDevice.h
)
add_dependencies(OpenUIDemo OpenUI)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE SDL3::SDL3)
find_package(glslang CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper)

if(OPENUI_ENABLE_OPENGL)
find_package(GLEW REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE GLEW::GLEW)
endif()

if(OPENUI_ENABLE_VULKAN)
# https://cmake.org/cmake/help/latest/module/FindVulkan.html
find_package(Vulkan REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE Vulkan::Vulkan)
endif()

if(OPENUI_ENABLE_DIRECTX)
find_package(directx-headers CONFIG REQUIRED)
find_package(directx12-agility CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE Microsoft::DirectX-Headers Microsoft::DirectX-Guids Microsoft::DirectX12-Agility dxgi d3dcompiler)
if(TARGET Microsoft::DirectX12-Agility)
	file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/D3D12")
	add_custom_command(TARGET OpenUIDemo POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Core,IMPORTED_LOCATION_RELEASE> $<TARGET_FILE_DIR:OpenUIDemo>/D3D12
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Layers,IMPORTED_LOCATION_DEBUG> $<TARGET_FILE_DIR:OpenUIDemo>/D3D12
		COMMAND_EXPAND_LISTS
	)
endif()
endif()

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

if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(OpenUIDemo PRIVATE OpenUI
		brotlicommon brotlidec brotlienc bz2d iconv
		cairo cairo-gobject cairo-script-interpreter
		ffi fontconfig freetyped fribidi GenericCodeGen
		glib-2.0 gmodule-2.0 gobject-2.0 gthread-2.0 gio-2.0 girepository-2.0
		harfbuzz-subset harfbuzz intl MachineIndependent OSDependent
		pango-1.0 pangocairo-1.0 pangoft2-1.0 pcre2-8 pcre2-16 pcre2-32 pcre2-posix pixman-1
		png png16d z
)
else()
target_link_libraries(OpenUIDemo PRIVATE OpenUI
		brotlicommon brotlidec brotlienc bz2 iconv
		cairo cairo-gobject cairo-script-interpreter
		ffi fontconfig freetype fribidi GenericCodeGen
		glib-2.0 gmodule-2.0 gobject-2.0 gthread-2.0 gio-2.0 girepository-2.0
		harfbuzz-subset harfbuzz intl MachineIndependent OSDependent
		pango-1.0 pangocairo-1.0 pangoft2-1.0 pcre2-8 pcre2-16 pcre2-32 pcre2-posix pixman-1
)
endif()

if(CMAKE_HOST_APPLE)
target_link_libraries(OpenUIDemo PRIVATE expat png png16 z)
endif()

if(CMAKE_HOST_WIN32)
target_link_libraries(OpenUIDemo PRIVATE pangowin32-1.0 Ws2_32 Dwrite)
endif()

endif()