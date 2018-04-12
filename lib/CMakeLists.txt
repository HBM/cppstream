project("hbm_daq_stream_client" CXX)

cmake_minimum_required(VERSION 3.0.2)

SET(WARN_SWITCHES "-Wall -Wextra -Winit-self -Wstrict-overflow=5 -Wpointer-arith -Wformat=2 -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -pedantic")



if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
  ## disable some warnings from gcc7, otherwise jsoncpp won't build!
  SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-implicit-fallthrough")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb" CACHE STRING "" FORCE)
IF(NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)
ENDIF()


set( INTERFACE_INCLUDE_DIR ../include/streamclient)

include_directories(${INTERFACE_INCLUDE_DIR})

SET( INTERFADE_HEADERS
    ${INTERFACE_INCLUDE_DIR}/bufferedreader.h
    ${INTERFACE_INCLUDE_DIR}/socketnonblocking.h
    ${INTERFACE_INCLUDE_DIR}/streamclient.h
    ${INTERFACE_INCLUDE_DIR}/signalcontainer.h
    ${INTERFACE_INCLUDE_DIR}/subscribedsignal.h
    ${INTERFACE_INCLUDE_DIR}/timeinfo.h
    ${INTERFACE_INCLUDE_DIR}/deltatimeinfo.h
    ${INTERFACE_INCLUDE_DIR}/types.h
)


set(STREAMCLIENT_SOURCES
  ${INTERFADE_HEADERS}
  timeinfo.cpp
  deltatimeinfo.cpp
  streamclient.cpp
  transportheader.cpp
  metainformation.cpp
  controller.cpp
  httppost.cpp
  signalcontainer.cpp
  subscribedsignal.cpp
  linux/socketnonblocking.cpp
  linux/bufferedreader.cpp
)

add_library(streamclient-static  STATIC ${STREAMCLIENT_SOURCES})
