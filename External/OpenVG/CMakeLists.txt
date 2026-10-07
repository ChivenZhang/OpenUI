# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.16)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
project ("ovg")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    add_compile_options(/utf-8)
else ()
    add_compile_options(-finput-charset=UTF-8)
endif ()

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(platform_i)
    set(platform_dir x64)
ELSE()
    set(platform_i 32)
    set(platform_dir x86)
ENDIF()
if(CMAKE_BUILD_TYPE MATCHES "Debug")
set(platform_dir "${platform_dir}d")
endif()
#设置执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/bin/${platform_dir})
#设置库文件输出目录
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
LINK_DIRECTORIES(${EXECUTABLE_OUTPUT_PATH})
include_directories("ovg")


option(OVG_ENABLE_SAMPLE "Enable Sample Compile" OFF)
option(OVG_ENABLE_SDL3_GPURenderer "Enable Default SDL3 GPURenderer" ON)

# ---- 查找 lib ----
find_package(SDL3 REQUIRED)
find_package(harfbuzz CONFIG REQUIRED) 
find_package(Fontconfig REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glm CONFIG REQUIRED)

# 包含子项目。
add_subdirectory ("ovg")
if (OVG_ENABLE_SAMPLE)
add_subdirectory ("sample")
endif()
