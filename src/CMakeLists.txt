project("hbm_daq_stream_examples" CXX)

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wextra -Winit-self -Wstrict-overflow=5 -Wpointer-arith -Wformat=2 -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -pedantic")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb" CACHE STRING "" FORCE)
IF(NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)
ENDIF()

include_directories(../include)

set(EXAMPLECLIENT_SOURCES
  exampleclient.cpp
)

add_executable( exampleclient.bin ${EXAMPLECLIENT_SOURCES} )

target_link_libraries( exampleclient.bin
  streamclient-static
  jsoncpp_lib_static
)

set(MULTICLIENT_SOURCES
  multiclient.cpp
)

install(TARGETS exampleclient.bin RUNTIME DESTINATION bin)



set(RAWCLIENT_SOURCES
  hbmstream_rawclient.cpp
)

add_executable( hbmstream_rawclient.bin ${RAWCLIENT_SOURCES} )

target_link_libraries( hbmstream_rawclient.bin
  streamclient-static
  jsoncpp_lib_static
)

set(MULTICLIENT_SOURCES
  multiclient.cpp
)

install(TARGETS hbmstream_rawclient.bin RUNTIME DESTINATION bin)




add_executable( multiclient.bin ${MULTICLIENT_SOURCES} )

target_link_libraries( multiclient.bin
  -pthread
  streamclient-static
  jsoncpp_lib_static
)


set(RAMP_SOURCES
  ramp.cpp
)

add_executable( ramp.bin ${RAMP_SOURCES} )

target_link_libraries( ramp.bin
  streamclient-static
  jsoncpp_lib_static
)

set(SAVESTREAM_SOURCES
  savestream.cpp
)

add_executable( savestream.bin ${SAVESTREAM_SOURCES} )

target_link_libraries( savestream.bin
  streamclient-static
  jsoncpp_lib_static
)

set(RECONNECT_SOURCES
  reconnect.cpp
)

add_executable( reconnect.bin ${RECONNECT_SOURCES} )

target_link_libraries( reconnect.bin
  -pthread
  streamclient-static
  jsoncpp_lib_static
)

set(STARTSTOP_SOURCES
  startstop.cpp
)

add_executable( startstop.bin ${STARTSTOP_SOURCES} )

target_link_libraries( startstop.bin
  -pthread
  streamclient-static
  jsoncpp_lib_static
)
