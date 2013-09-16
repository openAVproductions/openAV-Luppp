
#add_library (main main.cxx) 

find_package(PkgConfig)


pkg_check_modules(CAIRO cairo)
include_directories( ${CAIRO_INCLUDE_DIRS}  )
link_directories   ( ${CAIRO_LIBRARY_DIRS}  )
#pkg_check_modules(CAIROMM cairomm-1.0)
#include_directories( ${CAIROMM_INCLUDE_DIRS}  )
#link_directories   ( ${CAIROMM_LIBRARY_DIRS}  )

pkg_check_modules(NTK ntk)
include_directories( ${NTK_INCLUDE_DIRS}  )
link_directories   ( ${NTK_LIBRARY_DIRS}  )

pkg_check_modules(JACK jack)
include_directories( ${JACK_INCLUDE_DIRS}  )
link_directories   ( ${JACK_LIBRARY_DIRS}  )

pkg_check_modules(SNDFILE sndfile)
include_directories( ${SNDFILE_INCLUDE_DIRS}  )
link_directories   ( ${SNDFILE_LIBRARY_DIRS}  )

FILE(GLOB sources *.cxx  avtk/*.cxx cjson/*.c controller/*.cxx dsp/*.cxx observer/*.cxx state/*.cxx)
#FILE(GLOB sources tests/*.cxx)

#add_library( libluppp ${sources} )


#target_link_libraries( libluppp libluppp)
#target_link_libraries( libluppp ${CAIRO_LIBRARIES}   )
#target_link_libraries( libluppp ${NTK_LIBRARY_DIRS}  )
#target_link_libraries( libluppp ${Jack_LIBRARY_DIRS} )

add_executable (main ${sources} )

#target_link_libraries( main libluppp)
target_link_libraries( main ${JACK_LIBRARIES}     )
target_link_libraries( main ${NTK_LIBRARIES}      )
target_link_libraries( main ${CAIRO_LIBRARIES}    )
#target_link_libraries( main ${CAIROMM}  )
target_link_libraries( main ${SNDFILE_LIBRARIES}  )
