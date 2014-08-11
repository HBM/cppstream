project("HBM daq stream example")

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")


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

set(TESTRECONNECT_SOURCES
  tool/Reconnect.cpp
)

add_executable( reconnect.bin ${TESTRECONNECT_SOURCES} )

target_link_libraries( reconnect.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)

set(TESTMULTIPLE_SOURCES
  tool/Multiple.cpp
)

add_executable( multiple.bin ${TESTMULTIPLE_SOURCES} )

target_link_libraries( multiple.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
)


