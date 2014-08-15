project("HBM daq stream client")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -Wunused-result")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs -Wno-unused-but-set-variable")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

include_directories(.)
set(STREAMCLIENT_SOURCES
  Types.cpp
  StreamClient.cpp
  TransportHeader.cpp
  SocketNonblocking.h
  MetaInformation.cpp
  Controller.cpp
  HttpPost.cpp
  SubscribedSignal.cpp
  platform/linux/SocketNonblocking.cpp
  BufferedReader.cpp
)

add_library(streamclient-static  STATIC ${STREAMCLIENT_SOURCES})

