cmake_minimum_required(VERSION 3.11)

option(ENABLE_VCPKG OFF)

add_subdirectory("external/raylib/")
add_subdirectory("external/curve-x/")

#  Curve-X Editor executable
project(curve_editor_x)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE CURVE_EDITOR_X_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(curve_editor_x "${CURVE_EDITOR_X_SOURCES}")
target_include_directories(curve_editor_x PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_link_libraries(curve_editor_x PRIVATE curve_x raylib)