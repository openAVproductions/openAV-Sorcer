
cmake_minimum_required (VERSION 2.6)

project (sorcer)

set(SORCER_VERSION_MAJOR "1")
set(SORCER_VERSION_MINOR "1")
set(SORCER_VERSION_PATCH "1")

set(SORCER_VERSION "${SORCER_VERSION_MAJOR}.${SORCER_VERSION_MINOR}.${SORCER_VERSION_PATCH}")

option(RELEASE_BUILD  "Build for production usage" ON )

find_package(PkgConfig)

#SET(CMAKE_INSTALL_PREFIX "/usr" )

pkg_check_modules(LV2 lv2 REQUIRED)
include_directories( ${LV2_INCLUDE_DIRS}  )

pkg_check_modules(NTK ntk REQUIRED)
include_directories( ${NTK_INCLUDE_DIRS}  )
link_directories   ( ${NTK_LIBRARY_DIRS}  )

SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -Wl,-z,nodelete -Wl,--no-undefined")

IF(RELEASE_BUILD)
  SET(CMAKE_CXX_FLAGS "-g -Wall -Wno-unused-variable -msse2 -mfpmath=sse -ffast-math")
  SET(CMAKE_C_FLAGS "-g -Wall -W -Wno-unused-variable -msse2 -mfpmath=sse -ffast-math")
ENDIF()

## The following commands will re-generate the .cpp files from the FAUST .dsp,
## and extract the .ttl info from them, patch the custom UI URI's in, and copy
## them to the sorcer.lv2 folder.
##
## Note: This is *NOT* necessary for packaging, as the generated files are commited
## in the repo to remove the dependency on FAUST, and to easy packaging.
##
#  cd faust/ 
#  g++ -fPIC -shared -ffast-math -O3 -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"'  -Wl,-z,nodelete main.cpp -o sorcer.so
#  g++ -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"' main.cpp
#  ./a.out > sorcer.ttl
#  patch -u sorcer.ttl ../addUiRdf.patch
#  cp sorcer.so  ../sorcer.lv2/
#  cp sorcer.ttl ../sorcer.lv2/
#  cd ..

# tells FAUST to use the following URI for the plugin
ADD_DEFINITIONS( -DPLUGIN_URI="http://www.openavproductions.com/sorcer" )

# print config
string( ASCII 27 _esc)

function(COLOR_MESSAGE TEXT)
    if(CMAKE_COLOR_MAKEFILE)
        MESSAGE(${TEXT})
    else()
        string(REGEX REPLACE "${_esc}.[0123456789;]*m" "" __TEXT ${TEXT} )
        MESSAGE(${__TEXT})
    endif()
endfunction()

SET(green     "${_esc}[1;32m")
SET(blue      "${_esc}[1;34m")
SET(reset     "${_esc}[1;0m")

COLOR_MESSAGE( "
  ${blue}Sorcer Configuration : ${SORCER_VERSION}${reset}
-----------------------
* ${green}Install Directory${reset}               : ${CMAKE_INSTALL_PREFIX}\n" )
# /print config



FILE(GLOB sources faust/main.cpp +
  gui/sorcer_widget.cxx +
  gui/sorcer_ui.cxx )

ADD_LIBRARY(sorcer SHARED ${sources})

target_link_libraries( sorcer ${NTK_LIBRARIES}     )
target_link_libraries( sorcer ${SNDFILE_LIBRARIES} )

# Remove "lib" part before name (sorcer.so, not libsorcer.so)
set_target_properties(sorcer PROPERTIES PREFIX "")

# add the install targets
install (TARGETS sorcer DESTINATION lib/lv2/sorcer.lv2/)

# install .ttl files
install(FILES "sorcer.lv2/sorcer.ttl"     DESTINATION lib/lv2/sorcer.lv2/)
install(FILES "sorcer.lv2/manifest.ttl"  DESTINATION lib/lv2/sorcer.lv2/)
