if(OPENUI_ENABLE_SAMPLE)
add_executable (OpenUIDemo
        Cairo/CairoGLRender.cpp
		Cairo/CairoGLRender.h
		Cairo/CairoUIPainter.cpp
		Cairo/CairoUIPainter.h
		OpenGL/SDL3GLDevice.h
		main.cpp
		SDL3InputEnum.cpp
		SDL3InputEnum.h
)
add_dependencies(OpenUIDemo OpenUI)

find_package(GLEW REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE GLEW::GLEW)
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(OpenUIDemo PRIVATE SDL3::SDL3)

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