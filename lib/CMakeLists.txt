project("HBM daq stream client")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -Wunused-result")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs -Wno-unused-but-set-variable")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

set( INTERFACE_INCLUDE_DIR ../include/streamclient)

include_directories(${INTERFACE_INCLUDE_DIR})

SET( INTERFADE_HEADERS
    ${INTERFACE_INCLUDE_DIR}/bufferedreader.h
    ${INTERFACE_INCLUDE_DIR}/socketnonblocking.h
    ${INTERFACE_INCLUDE_DIR}/streamclient.h
    ${INTERFACE_INCLUDE_DIR}/subscribedsignal.h
    ${INTERFACE_INCLUDE_DIR}/types.h
)


set(STREAMCLIENT_SOURCES
  ${INTERFADE_HEADERS}
  types.cpp
  streamclient.cpp
  transportheader.cpp
  metainformation.cpp
  controller.cpp
  httppost.cpp
  subscribedsignal.cpp
  platform/linux/socketnonblocking.cpp
  bufferedreader.cpp
)

add_library(streamclient-static  STATIC ${STREAMCLIENT_SOURCES})
