cmake_minimum_required(VERSION 3.11)

#  Curve-X library
project(curve_x)
set(CMAKE_CXX_STANDARD 17)

#  List all .cpp files
file(GLOB_RECURSE CURVE_X_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

#  Declare library with sources and headers
add_library(curve_x)
target_include_directories(curve_x PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(curve_x PRIVATE "${CURVE_X_SOURCES}")