if(OPENUI_ENABLE_SAMPLE)

if(OPENUI_ENABLE_SDLGPU)
	add_definitions(-DOPENUI_ENABLE_SDLGPU)
endif ()
if(CMAKE_HOST_WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

file(GLOB_RECURSE OPENUI_SAMPLE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable (Sample ${OPENUI_SAMPLE_SRC}
		SDLGPU/SDLGPUEffector.cpp
		SDLGPU/SDLGPUEffector.h
		SDLGPU/SDLGPUMerger.cpp
		SDLGPU/SDLGPUMerger.h
        SDLGPU/SDLGPUInverter.cpp
        SDLGPU/SDLGPUInverter.h)
add_dependencies(Sample OpenUI OpenVG)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(Sample PRIVATE SDL3::SDL3)

if(OPENUI_ENABLE_OPENGL)
find_package(GLEW REQUIRED)
target_link_libraries(Sample PRIVATE GLEW::GLEW)
find_package(glslang CONFIG REQUIRED)
target_link_libraries(Sample PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper)
endif()

if(OPENUI_ENABLE_VULKAN)
# https://cmake.org/cmake/help/latest/module/FindVulkan.html
find_package(Vulkan REQUIRED)
target_link_libraries(Sample PRIVATE Vulkan::Vulkan)
endif()

if(OPENUI_ENABLE_DIRECTX)
find_package(directx-headers CONFIG REQUIRED)
find_package(directx12-agility CONFIG REQUIRED)
target_link_libraries(Sample PRIVATE Microsoft::DirectX-Headers Microsoft::DirectX-Guids Microsoft::DirectX12-Agility dxgi d3dcompiler)
if(TARGET Microsoft::DirectX12-Agility)
	file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/D3D12")
	add_custom_command(TARGET Sample POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Core,IMPORTED_LOCATION_RELEASE> $<TARGET_FILE_DIR:Sample>/D3D12
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Layers,IMPORTED_LOCATION_DEBUG> $<TARGET_FILE_DIR:Sample>/D3D12
		COMMAND_EXPAND_LISTS
	)
endif()
endif()

if(OPENUI_ENABLE_METAL)
target_include_directories(Sample PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/Metal/metal-cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Metal/metal-cpp-extensions"
)
target_link_libraries(Sample PRIVATE
		"-framework Metal"
		"-framework MetalKit"
		"-framework AppKit"
		"-framework Foundation"
		"-framework QuartzCore"
)
endif()

if(OPENUI_ENABLE_SDLGPU)

endif()

target_include_directories(Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
target_include_directories(Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo)
target_include_directories(Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz)
target_include_directories(Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0)
target_include_directories(Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0)
target_include_directories(Sample PUBLIC ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_directories(Sample PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
else()
target_link_directories(Sample PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(Sample PRIVATE OpenUI OpenVG

)
else()
target_link_libraries(Sample PRIVATE OpenUI OpenVG

)
endif()

if(CMAKE_HOST_APPLE)
target_link_libraries(Sample PRIVATE expat GenericCodeGen MachineIndependent OSDependent pcre2-8 pcre2-16 pcre2-32 pcre2-posix png png16 z)
endif()

endif()