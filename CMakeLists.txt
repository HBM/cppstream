project(cppstream)

cmake_minimum_required(VERSION 2.8)

include_directories(.)

set(SOURCES
  TransportHeader.cpp
  Stream.cpp
  SocketNonblocking.h
  platform/linux/SocketNonblocking.cpp
  BufferedReader.cpp
)

add_executable( cppstream.bin ${SOURCES} )
