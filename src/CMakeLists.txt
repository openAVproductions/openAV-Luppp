
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

# Check build type, adding coverage flags if needed
IF(BUILD_TESTS)
  ADD_DEFINITIONS(-DBUILD_TESTS)
  ADD_DEFINITIONS(-DBUILD_COVERAGE_TEST)
  
  SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -Wno-unused-variable -fprofile-arcs -ftest-coverage")
  SET(CMAKE_C_FLAGS "-g -O0 -Wall -Wextra -W -Wno-unused-variable -fprofile-arcs -ftest-coverage")
  SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
ENDIF(BUILD_TESTS)

IF(RELEASE_BUILD)
  SET(CMAKE_CXX_FLAGS "-g -O2 -Wall  -Wno-unused-variable -ffast-math")
  SET(CMAKE_C_FLAGS "-g -O2 -Wall -W  -Wno-unused-variable -ffast-math")
ENDIF(RELEASE_BUILD)


# Add all files
FILE(GLOB sources *.cxx  avtk/*.cxx cjson/*.c controller/*.cxx dsp/*.cxx observer/*.cxx state/*.cxx tests/*.cxx)

# Compile binary
add_executable (luppp ${sources} )


# Linking
target_link_libraries( luppp ${JACK_LIBRARIES}     )
target_link_libraries( luppp ${NTK_LIBRARIES}      )
target_link_libraries( luppp ${CAIRO_LIBRARIES}    )
target_link_libraries( luppp ${SNDFILE_LIBRARIES}  )

# Check build type, linking with gcov for code analysis if needed
IF(BUILD_TESTS)
  target_link_libraries(luppp gcov)
ENDIF(BUILD_TESTS)
