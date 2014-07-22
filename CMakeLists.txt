project(cppstream)

cmake_minimum_required(VERSION 2.8)

include_directories(.)
set(SOURCES
  TransportHeader.cpp
  Stream.cpp
  SocketNonblocking.h
  MetaInformation.cpp
  Controller.cpp
  HttpPost.cpp
  platform/linux/SocketNonblocking.cpp
  BufferedReader.cpp
)


add_executable( cppstream.bin ${SOURCES} )

target_link_libraries( cppstream.bin
  jsoncpp
)
