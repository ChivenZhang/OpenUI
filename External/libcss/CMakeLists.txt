cmake_minimum_required(VERSION 3.10)

project("libcss")

include_directories(
  libcss/include
  libparserutils/include
  libwapcaplet/include
)

add_definitions(-DWITHOUT_ICONV_FILTER)
add_subdirectory(libwapcaplet)
add_subdirectory(libparserutils)
add_subdirectory(libcss)
add_subdirectory(libdom)
