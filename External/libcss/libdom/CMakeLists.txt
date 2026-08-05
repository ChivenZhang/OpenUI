
file(GLOB_RECURSE LIBDOM_SOURCE_FILES "src/*.c" "src/*.h" "include/*.h")

include_directories("include" "src")
add_library(dom STATIC ${LIBDOM_SOURCE_FILES})