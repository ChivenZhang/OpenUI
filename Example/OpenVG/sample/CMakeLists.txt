 
# ---- 源文件 ----
set(OVG_SRC
    main_vg.cpp 
) 
set(OVG_SRC_C
    main_c.c
) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# ---- 可执行文件 ----
add_executable(ovg ${OVG_SRC})
target_link_libraries(ovg PRIVATE libovg SDL3::SDL3 )
add_executable(ovg_c ${OVG_SRC_C})
target_link_libraries(ovg_c PRIVATE libovg SDL3::SDL3 )
