
# Find depend libraries
find_package(PkgConfig)

pkg_check_modules(CAIRO cairo)
include_directories( ${CAIRO_INCLUDE_DIRS}  )
link_directories   ( ${CAIRO_LIBRARY_DIRS}  )

pkg_check_modules(NTK ntk)
include_directories( ${NTK_INCLUDE_DIRS}  )
link_directories   ( ${NTK_LIBRARY_DIRS}  )

pkg_check_modules(JACK jack)
include_directories( ${JACK_INCLUDE_DIRS}  )
link_directories   ( ${JACK_LIBRARY_DIRS}  )

pkg_check_modules(SNDFILE sndfile)
include_directories( ${SNDFILE_INCLUDE_DIRS}  )
link_directories   ( ${SNDFILE_LIBRARY_DIRS}  )

# Add flags
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

#SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")
#SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

# Add files
FILE(GLOB sources *.cxx  avtk/*.cxx cjson/*.c controller/*.cxx dsp/*.cxx observer/*.cxx state/*.cxx tests/*.cxx)

add_executable (luppp ${sources} )

# Linking
target_link_libraries( luppp ${JACK_LIBRARIES}     )
target_link_libraries( luppp ${NTK_LIBRARIES}      )
target_link_libraries( luppp ${CAIRO_LIBRARIES}    )
target_link_libraries( luppp ${SNDFILE_LIBRARIES}  )

target_link_libraries( luppp gcov)
