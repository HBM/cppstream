project("HBM daq stream examples")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wextra -Weffc++ -Winit-self -Wstrict-overflow=5 -Wpointer-arith -Wformat=2 -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wcast-qual -Wwrite-strings")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

include_directories(../include)

set(EXAMPLECLIENT_SOURCES
  exampleclient.cpp
)

add_executable( exampleclient.bin ${EXAMPLECLIENT_SOURCES} )

target_link_libraries( exampleclient.bin
  streamclient-static
  jsoncpp
)

set(RECONNECT_SOURCES
  reconnect.cpp
)

add_executable( reconnect.bin ${RECONNECT_SOURCES} )

target_link_libraries( reconnect.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(MULTIPLE_SOURCES
  multiple.cpp
)

add_executable( multiple.bin ${MULTIPLE_SOURCES} )

target_link_libraries( multiple.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)
