 
FILE(GLOB_RECURSE TESS2_SRC 
  tess2/src/*.c 
)
set(OVG_SOURCES 
    ovg.cpp
    ${TESS2_SRC}
    ovg_fonts.cpp   
)

if(OVG_ENABLE_SDL3_GPURenderer)
list(APPEND OVG_SOURCES ovg_renderer_sdl3.cpp) 
endif()

find_package(harfbuzz CONFIG REQUIRED) 
find_package(Fontconfig REQUIRED) 
pkg_check_modules(HARFBUZZ_RASTER REQUIRED IMPORTED_TARGET harfbuzz-raster)

add_library(libovg STATIC ${OVG_SOURCES})
target_link_libraries(libovg PUBLIC glm::glm-header-only harfbuzz::harfbuzz PkgConfig::HARFBUZZ_RASTER Fontconfig::Fontconfig)

target_include_directories(libovg PRIVATE "tess2/Include"})
