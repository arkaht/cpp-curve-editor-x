cmake_minimum_required(VERSION 3.11)

#  Curve-X Editor executable
project(curve_editor_x)

set(CMAKE_CXX_STANDARD 20)

#  Force Unicode 
add_compile_definitions(UNICODE)
message("Forcing Unicode over Multi-byte (using Windows.h)")

add_subdirectory("external")

file(GLOB_RECURSE CURVE_EDITOR_X_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(curve_editor_x "${CURVE_EDITOR_X_SOURCES}")
target_include_directories(curve_editor_x PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_link_libraries(curve_editor_x PRIVATE curve_x raylib)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/")