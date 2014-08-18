project("HBM daq stream client")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -Wunused-result")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs -Wno-unused-but-set-variable")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

set( INTERFACE_INCLUDE_DIR ../include/StreamClient)

include_directories(${INTERFACE_INCLUDE_DIR})

SET( INTERFADE_HEADERS
    ${INTERFACE_INCLUDE_DIR}/BufferedReader.h
    ${INTERFACE_INCLUDE_DIR}/SocketNonblocking.h
    ${INTERFACE_INCLUDE_DIR}/StreamClient.h
    ${INTERFACE_INCLUDE_DIR}/SubscribedSignal.h
    ${INTERFACE_INCLUDE_DIR}/Types.h
)


set(STREAMCLIENT_SOURCES
  ${INTERFADE_HEADERS}
  Types.cpp
  StreamClient.cpp
  TransportHeader.cpp
  MetaInformation.cpp
  Controller.cpp
  HttpPost.cpp
  SubscribedSignal.cpp
  platform/linux/SocketNonblocking.cpp
  BufferedReader.cpp
)

add_library(streamclient-static  STATIC ${STREAMCLIENT_SOURCES})
