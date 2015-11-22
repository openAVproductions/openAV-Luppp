
# Detect platform
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message( STATUS "Architecture:  ${ARCHITECTURE}" )

# Find depend libraries
find_package(PkgConfig)

pkg_check_modules(CAIRO cairo REQUIRED)
include_directories( ${CAIRO_INCLUDE_DIRS}  )
link_directories   ( ${CAIRO_LIBRARY_DIRS}  )

pkg_check_modules(NTK ntk REQUIRED)
include_directories( ${NTK_INCLUDE_DIRS}  )
link_directories   ( ${NTK_LIBRARY_DIRS}  )

pkg_check_modules(LIBLO liblo REQUIRED)
include_directories( ${LIBLO_INCLUDE_DIRS}  )
link_directories   ( ${LIBLO_LIBRARY_DIRS}  )

pkg_check_modules(JACK jack REQUIRED)
include_directories( ${JACK_INCLUDE_DIRS}  )
link_directories   ( ${JACK_LIBRARY_DIRS}  )

pkg_check_modules(SNDFILE sndfile REQUIRED)
include_directories( ${SNDFILE_INCLUDE_DIRS}  )
link_directories   ( ${SNDFILE_LIBRARY_DIRS}  )

pkg_check_modules(SAMPLERATE samplerate REQUIRED)
include_directories( ${SAMPLERATE_INCLUDE_DIRS}  )
link_directories   ( ${SAMPLERATE_LIBRARY_DIRS}  )

# needed for setting icon in WM
pkg_check_modules(X11 x11 REQUIRED)
include_directories( ${X11_INCLUDE_DIRS}  )
link_directories   ( ${X11_LIBRARY_DIRS}  )

# Check build type, adding coverage flags if needed
IF(BUILD_TESTS)
  ADD_DEFINITIONS(-DBUILD_TESTS)
  ADD_DEFINITIONS(-DBUILD_COVERAGE_TEST)
  
  SET(CMAKE_CXX_FLAGS " -g -Wall -Wextra -Wno-unused-variable -fprofile-arcs -ftest-coverage ")
  SET(CMAKE_C_FLAGS " -g -Wall -Wextra -W -Wno-unused-variable -fprofile-arcs -ftest-coverage ")
  SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
ENDIF(BUILD_TESTS)

IF(RELEASE_BUILD)
  SET(CMAKE_CXX_FLAGS " -g -Wall -Wno-unused-variable ")
  SET(CMAKE_C_FLAGS " -g -Wall -W -Wno-unused-variable ")
ENDIF(RELEASE_BUILD)

if( ${ARCHITECTURE} STREQUAL "x86_64" )
  set(CMAKE_C_FLAGS   ${CMAKE_C_FLAGS}"-msse2" )
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}"-msse2" )
else()
  # Can set i686 / ARM stuff here
endif()

# Add all files
FILE(GLOB sources *.cxx  avtk/*.cxx cjson/*.c controller/*.cxx dsp/*.cxx observer/*.cxx state/*.cxx tests/*.cxx)

# Compile binary
add_executable (luppp version.hxx ${sources} )


# Linking
target_link_libraries( luppp ${JACK_LIBRARIES}       )
target_link_libraries( luppp ${LIBLO_LIBRARIES}      )
target_link_libraries( luppp ${NTK_LIBRARIES}        )
target_link_libraries( luppp ${CAIRO_LIBRARIES}      )
target_link_libraries( luppp ${SNDFILE_LIBRARIES}    )
target_link_libraries( luppp ${SAMPLERATE_LIBRARIES} )
target_link_libraries( luppp ${X11_LIBRARIES}        )

# Check build type, linking with gcov for code analysis if needed
IF(BUILD_TESTS)
  target_link_libraries(luppp gcov)
ENDIF(BUILD_TESTS)

# add the install targets
install (TARGETS luppp DESTINATION bin)

