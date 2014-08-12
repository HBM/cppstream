project("HBM daq stream example")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -Wunused-result")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs -Wno-unused-but-set-variable")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src )

include_directories(.)
set(STREAMCLIENT_SOURCES
  src/Types.cpp
  src/StreamClient.cpp
  src/TransportHeader.cpp
  src/SocketNonblocking.h
  src/MetaInformation.cpp
  src/Controller.cpp
  src/HttpPost.cpp
  src/SubscribedSignal.cpp
  src/platform/linux/SocketNonblocking.cpp
  src/BufferedReader.cpp
)


add_library(streamclient-static  STATIC ${STREAMCLIENT_SOURCES})



set(EXAMPLECLIENT_SOURCES
  src/tool/ExampleClient.cpp
)

add_executable( exampleclient.bin ${EXAMPLECLIENT_SOURCES} )

target_link_libraries( exampleclient.bin
  streamclient-static
  jsoncpp
)

set(RECONNECT_SOURCES
  src/tool/Reconnect.cpp
)

add_executable( reconnect.bin ${RECONNECT_SOURCES} )

target_link_libraries( reconnect.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(MULTIPLE_SOURCES
  src/tool/Multiple.cpp
)

add_executable( multiple.bin ${MULTIPLE_SOURCES} )

target_link_libraries( multiple.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(TEST_HTTPPOST_SOURCES
  src/HttpPost.cpp
  src/platform/linux/SocketNonblocking.cpp
  src/BufferedReader.cpp
  test/TestHttpPost.cpp
)

add_executable( testhttppost.bin ${TEST_HTTPPOST_SOURCES} )

target_link_libraries( testhttppost.bin
  jsoncpp
)

set(TEST_STREAMCLIENT_SOURCES
  test/TestStreamClient.cpp
)

add_executable( teststreamclient.bin ${TEST_STREAMCLIENT_SOURCES} )

target_link_libraries( teststreamclient.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

