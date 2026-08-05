
file(GLOB_RECURSE OPENUI_SOURCE_FILES "*.cpp" "*.h")
add_library(OpenUI STATIC ${OPENUI_SOURCE_FILES})

find_package(yoga CONFIG REQUIRED)
target_link_libraries(OpenUI PRIVATE yoga::yogacore)

find_package(Stb REQUIRED)
target_include_directories(OpenUI PRIVATE ${Stb_INCLUDE_DIR})