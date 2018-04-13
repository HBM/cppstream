project("hbm_daq_stream_example")

cmake_minimum_required(VERSION 3.0.2)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
INCLUDE(generate_doxygen)

enable_testing()

include_directories("jsoncpp/include")

add_subdirectory("jsoncpp")
add_subdirectory("lib")
add_subdirectory("src")
add_subdirectory("test")
