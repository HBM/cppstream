project("HBM daq stream example")

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")


include_directories(.)
set(STREAMCLIENT_SOURCES
  Types.cpp
  Stream.cpp
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
  ExampleClient.cpp
)

add_executable( exampleclient.bin ${EXAMPLECLIENT_SOURCES} )

target_link_libraries( exampleclient.bin
  streamclient-static
  jsoncpp
)

set(TESTRECONNECT_SOURCES
  test/TestReconnect.cpp
)

add_executable( testreconnect.bin ${TESTRECONNECT_SOURCES} )

target_link_libraries( testreconnect.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(TESTMULTIPLE_SOURCES
  test/TestMultiple.cpp
)

add_executable( testmultiple.bin ${TESTMULTIPLE_SOURCES} )

target_link_libraries( testmultiple.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)


