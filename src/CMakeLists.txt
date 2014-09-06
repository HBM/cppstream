project("HBM daq stream examples" CXX)

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wextra -Weffc++ -Winit-self -Wstrict-overflow=5 -Wpointer-arith -Wformat=2 -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wcast-qual -Wwrite-strings")

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
