project("hbm_daq_stream_tests" CXX)

cmake_minimum_required(VERSION 2.8)

SET(WARN_SWITCHES "-Wall -Wextra -Weffc++ -Winit-self -Wstrict-overflow=5 -Wpointer-arith -Wformat=2 -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wcast-qual -Wwrite-strings")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb" CACHE STRING "" FORCE)
IF(NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)
ENDIF()

include_directories(../include)

include_directories(/usr/include/jsoncpp/)

set(TEST_STREAMCLIENT_SOURCES
  teststreamclient.cpp
)

add_executable( teststreamclient.bin ${TEST_STREAMCLIENT_SOURCES} )

target_link_libraries( teststreamclient.bin
  streamclient-static
  jsoncpp
  boost_unit_test_framework
)

add_test(test teststreamclient.bin)
