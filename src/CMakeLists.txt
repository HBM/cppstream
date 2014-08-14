project("HBM daq stream example")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -Wunused-result")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs -Wno-unused-but-set-variable")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src )

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



set(EXAMPLECLIENT_SOURCES
  tool/ExampleClient.cpp
)

add_executable( exampleclient.bin ${EXAMPLECLIENT_SOURCES} )

target_link_libraries( exampleclient.bin
  streamclient-static
  jsoncpp
)

set(RECONNECT_SOURCES
  tool/Reconnect.cpp
)

add_executable( reconnect.bin ${RECONNECT_SOURCES} )

target_link_libraries( reconnect.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(MULTIPLE_SOURCES
  tool/Multiple.cpp
)

add_executable( multiple.bin ${MULTIPLE_SOURCES} )

target_link_libraries( multiple.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(TEST_HTTPPOST_SOURCES
  HttpPost.cpp
  platform/linux/SocketNonblocking.cpp
  BufferedReader.cpp
  test/TestHttpPost.cpp
)

#add_executable( testhttppost.bin ${TEST_HTTPPOST_SOURCES} )
#
#target_link_libraries( testhttppost.bin
#  jsoncpp
#)
#
#set(TEST_STREAMCLIENT_SOURCES
#  test/TestStreamClient.cpp
#)
#
#add_executable( teststreamclient.bin ${TEST_STREAMCLIENT_SOURCES} )
#
#target_link_libraries( teststreamclient.bin
#  streamclient-static
#  jsoncpp
#  boost_system
#  boost_thread
#  boost_unit_test_framework
#)
#
