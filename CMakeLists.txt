cmake_minimum_required (VERSION 2.6)
project (RadarClutterSimulator)

# Project Version Number
set (RadarClutterSimulator_VERSION_MAJOR 0)
set (RadarClutterSimulator_VERSION_MINOR 1)

# C++11
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} ${CMAKE_CXX_COMPILER} ")

include_directories(include)
FILE(GLOB SRCFILES src/*.cpp)
add_executable(clutter_sim src/cli_interface.cpp src/dem_parser/dem_parser.cpp src/dem_parser/elevation_reader.cpp src/dem_parser/shadowing.cpp src/dem_parser/map_exporter.cpp src/dem_parser/terrain_slope.cpp src/dem_parser/threevector.cpp src/echo_sim/clutter_coefficient.cpp src/echo_sim/conversion.cpp src/echo_sim/echo_sim.cpp src/echo_sim/random.cpp src/echo_sim/antenna_pattern.cpp)
