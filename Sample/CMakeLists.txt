if(OPENUI_ENABLE_SAMPLE)

if(OPENUI_ENABLE_OPENGL)
	add_definitions(-DOPENUI_ENABLE_OPENGL)
endif ()
if(OPENUI_ENABLE_VULKAN)
	add_definitions(-DOPENUI_ENABLE_VULKAN)
endif ()

add_executable (OpenUIDemo
		OpenGL/CairoGLRender.cpp
		OpenGL/CairoGLRender.h
		Cairo/CairoUIPainter.cpp
		Cairo/CairoUIPainter.h
		OpenGL/SDL3GLDevice.h
		main.cpp
		SDL3InputEnum.cpp
		SDL3InputEnum.h
		Vulkan/CairoVKRender.h
		Vulkan/CairoVKRender.cpp
)
add_dependencies(OpenUIDemo OpenUI)

find_package(GLEW REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE GLEW::GLEW)
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE SDL3::SDL3)
# https://cmake.org/cmake/help/latest/module/FindVulkan.html
find_package(Vulkan REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE Vulkan::Vulkan)
find_package(glslang CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper)

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

target_link_libraries(OpenUIDemo PRIVATE
		OpenUI
		gmodule-2.0 gobject-2.0 gthread-2.0
		cairo cairo-gobject cairo-script-interpreter
		pango-1.0 pangocairo-1.0 pangoft2-1.0 pangowin32-1.0
		Ws2_32 Dwrite
)

endif()