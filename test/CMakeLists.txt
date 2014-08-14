project("HBM daq stream example tests")

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -Wunused-result")
SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-unused-local-typedefs -Wno-unused-but-set-variable")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

include_directories("../src")

set(TEST_STREAMCLIENT_SOURCES
  TestStreamClient.cpp
)

add_executable( teststreamclient.bin ${TEST_STREAMCLIENT_SOURCES} )

target_link_libraries( teststreamclient.bin
  streamclient-static
  jsoncpp
  boost_system
  boost_thread
  boost_unit_test_framework
)

