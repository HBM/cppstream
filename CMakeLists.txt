project(cppstream)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")


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
