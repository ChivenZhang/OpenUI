cmake_minimum_required(VERSION 3.10...5.0)
project(OpenUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(OPENUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "OpenUI Source Path")
set(OPENUI_EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/External CACHE STRING "OpenUI External Path")
option(OPENUI_ENABLE_SAMPLE "Enable Sample Compile" OFF)

if (MSVC)
    add_compile_options(/utf-8)
else ()
    add_compile_options(-finput-charset=UTF-8)
endif ()

include_directories(${OPENUI_SOURCE_DIR})
include_directories(${OPENUI_SOURCE_DIR}/Vector/SDLGPU)
add_subdirectory(OpenVG)
add_subdirectory(Vector/SDLGPU)
add_subdirectory(OpenUI)


if (OPENUI_ENABLE_SAMPLE)

    find_program(SLANGC_EXECUTABLE slangc)
    find_program(GLSLANG_VALIDATOR glslangValidator)
    if (NOT SLANGC_EXECUTABLE)
        message(FATAL_ERROR "slangc not found")
    endif ()
    if (NOT GLSLANG_VALIDATOR)
        message(FATAL_ERROR "glslangValidator not found")
    endif ()

    set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shader)
    set(SPV_OUT_DIR    ${CMAKE_CURRENT_BINARY_DIR}/Shader)

    file(MAKE_DIRECTORY ${SPV_OUT_DIR})

    file(GLOB SLANG_FILES "${SHADER_SRC_DIR}/*.slang")

    set(SHADER_OUTPUTS)

    foreach(SLANG_FILE ${SLANG_FILES})
        get_filename_component(SHADER_NAME ${SLANG_FILE} NAME_WE)

        set(VERT_GLSL ${SPV_OUT_DIR}/${SHADER_NAME}_vert.glsl)
        set(VERT_H    ${SPV_OUT_DIR}/${SHADER_NAME}.vert.h)
        set(FRAG_GLSL ${SPV_OUT_DIR}/${SHADER_NAME}_frag.glsl)
        set(FRAG_H    ${SPV_OUT_DIR}/${SHADER_NAME}.frag.h)

        message(STATUS "Compile ${SLANG_FILE} \n\t=> ${VERT_H}\n\t=> ${FRAG_H}")

        add_custom_command(
                OUTPUT "${VERT_GLSL}"
                COMMAND ${SLANGC_EXECUTABLE} ${SLANG_FILE} -target glsl -profile glsl_450 -force-glsl-scalar-layout -entry VSMain -D DOUBLESIDEDCOLOR -D ID_INSTANCING=1 -o ${VERT_GLSL}
                DEPENDS "${SLANG_FILE}"
                COMMENT "[${SHADER_NAME}] slang → GLSL (vert)"
                VERBATIM
        )
        add_custom_command(
                OUTPUT "${VERT_H}"
                COMMAND ${GLSLANG_VALIDATOR} ${VERT_GLSL} -V -S vert --source-entrypoint main -e VSMain --vn ${SHADER_NAME}_vert -o ${VERT_H}
                DEPENDS "${VERT_GLSL}"
                COMMENT "[${SHADER_NAME}] GLSL → SPIR-V header (vert)"
                VERBATIM
        )

        add_custom_command(
                OUTPUT "${FRAG_GLSL}"
                COMMAND ${SLANGC_EXECUTABLE} ${SLANG_FILE} -target glsl -profile glsl_450 -force-glsl-scalar-layout -entry FSMain -D DOUBLESIDEDCOLOR -D ID_INSTANCING=1 -o ${FRAG_GLSL}
                DEPENDS "${SLANG_FILE}"
                COMMENT "[${SHADER_NAME}] slang → GLSL (frag)"
                VERBATIM
        )
        add_custom_command(
                OUTPUT "${FRAG_H}"
                COMMAND ${GLSLANG_VALIDATOR} ${FRAG_GLSL} -V -S frag --source-entrypoint main -e FSMain --vn ${SHADER_NAME}_frag -o ${FRAG_H}
                DEPENDS "${FRAG_GLSL}"
                COMMENT "[${SHADER_NAME}] GLSL → SPIR-V header (frag)"
                VERBATIM
        )

        list(APPEND SHADER_OUTPUTS
                "${VERT_H}"
                "${FRAG_H}"
        )
    endforeach()

    add_custom_target(SampleShaders ALL
            DEPENDS ${SHADER_OUTPUTS}
    )

    add_subdirectory(Sample)

    add_dependencies(Sample SampleShaders)

    target_include_directories(Sample PUBLIC ${SPV_OUT_DIR})
endif ()