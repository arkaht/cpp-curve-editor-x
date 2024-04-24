cmake_minimum_required(VERSION 3.11)

#  Curve-X Editor executable
project(curve_editor_x)
set(CMAKE_CXX_STANDARD 17)

#  Force Unicode 
add_compile_definitions(UNICODE)
message("Forcing Unicode over Multi-byte (using Windows.h)")

#  Execute other CMakeLists.txt
add_subdirectory("external")

#  List all .cpp files
file(GLOB_RECURSE CURVE_EDITOR_X_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

#  Declare executable with sources, headers and linked libraries
add_executable(curve_editor_x "${CURVE_EDITOR_X_SOURCES}")
target_include_directories(curve_editor_x PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/")
target_link_libraries(curve_editor_x PRIVATE curve_x raylib)

#  Additional include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/")

#  List include directories
# message("Included directories: ${INCLUDE_DIRECTORIES}")
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()