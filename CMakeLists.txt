
cmake_minimum_required (VERSION 2.6)

project (LUPPP)

set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)


#option(BUILD_TESTS "Build test version" OFF)

add_definitions( -DBUILD_TESTS          )
add_definitions( -DBUILD_COVERAGE_TEST  )


set(CMAKE_VERBOSE_MAKEFILE on)

#set(CMAKE_FILES_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (src)
