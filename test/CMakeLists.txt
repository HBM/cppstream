project("hbm_daq_stream_tests" CXX)

cmake_minimum_required(VERSION 3.0.2)

enable_testing()

SET(WARN_SWITCHES "-Wall -Wextra -Winit-self -Wstrict-overflow=5 -Wpointer-arith -Wformat=2 -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wcast-qual -Wwrite-strings -pedantic")

if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
  ## disable some warnings from gcc7, otherwise jsoncpp won't build!
  SET(WARN_SWITCHES "${WARN_SWITCHES} -Wno-implicit-fallthrough")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ${WARN_SWITCHES}")

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb" CACHE STRING "" FORCE)
IF(NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)
ENDIF()

include_directories(../include)

set(TEST_STREAMCLIENT_SOURCES
  teststreamclient.cpp
)

add_executable( teststreamclient.bin ${TEST_STREAMCLIENT_SOURCES} )

target_link_libraries( teststreamclient.bin
  streamclient-static
  jsoncpp_lib_static
  boost_unit_test_framework
)

set(TEST_TIME_SOURCES
  testtime.cpp
)

add_executable( testtime.bin ${TEST_TIME_SOURCES} )

target_link_libraries( testtime.bin
  streamclient-static
  jsoncpp_lib_static
  boost_unit_test_framework
)


add_test(streamclient.test teststreamclient.bin)
add_test(time.test teststreamclient.bin)
