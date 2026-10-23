
file(GLOB_RECURSE OPENUI_SOURCE_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")
add_library(OpenUI STATIC ${OPENUI_SOURCE_FILES})

target_link_libraries(OpenUI PRIVATE yogacore)

find_package(lexbor CONFIG REQUIRED)
if(BUILD_SHARED_LIBS)
target_link_libraries(OpenUI PRIVATE lexbor::lexbor)
else()
target_link_libraries(OpenUI PRIVATE lexbor::lexbor_static)
endif()

find_package(Stb REQUIRED)
target_include_directories(OpenUI PRIVATE ${Stb_INCLUDE_DIR})

find_package(glm CONFIG REQUIRED)
target_link_libraries(OpenUI PRIVATE glm::glm-header-only)