
cmake_minimum_required (VERSION 2.6)

project (LUPPP)

set(LUPPP_VERSION_MAJOR "1")
set(LUPPP_VERSION_MINOR "1")
set(LUPPP_VERSION_PATCH "1")

set(LUPPP_VERSION "${LUPPP_VERSION_MAJOR}.${LUPPP_VERSION_MINOR}.${LUPPP_VERSION_PATCH}")

option(BUILD_TESTS "Build test version" OFF)


#set(CMAKE_VERBOSE_MAKEFILE on)

#set(CMAKE_FILES_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#execute_process(COMMAND "date +\"%d-%m-%Y\"` `git log --pretty=format:\"%h\" | head -n1`"

#execute_process(COMMAND "git log --pretty=format:\"%h\" | head -n1"

execute_process(COMMAND git log --oneline -n1
    OUTPUT_VARIABLE _output OUTPUT_STRIP_TRAILING_WHITESPACE)
    file(WRITE src/version.hxx "#define GIT_VERSION \"${_output}\"")


add_subdirectory (src)
