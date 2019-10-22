cmake_minimum_required (VERSION 2.6)
project (RadarClutterSimulator)

# Project Version Number
set (RadarClutterSimulator_VERSION_MAJOR 0)
set (RadarClutterSimulator_VERSION_MINOR 1)

# C++11
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} ${CMAKE_CXX_COMPILER} ")

include_directories(include)

add_executable(RadarClutterSimulator src/cli_interface.cpp)

