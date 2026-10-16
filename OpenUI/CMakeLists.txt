
file(GLOB_RECURSE OPENUI_SOURCE_FILES "*.cpp" "*.h")
add_library(OpenUI STATIC ${OPENUI_SOURCE_FILES})

target_link_libraries(OpenUI PRIVATE yogacore)

find_package(lexbor CONFIG REQUIRED)
target_link_libraries(OpenUI PRIVATE lexbor::lexbor)

find_package(Stb REQUIRED)
target_include_directories(OpenUI PRIVATE ${Stb_INCLUDE_DIR})

find_package(glm CONFIG REQUIRED)
target_link_libraries(OpenUI PRIVATE glm::glm-header-only)