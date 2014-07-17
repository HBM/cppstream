project(cppstream)

cmake_minimum_required(VERSION 2.8)

set(SOURCES
  stream.cpp
)

add_executable( cppstream.bin ${SOURCES} )

