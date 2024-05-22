cmake_minimum_required(VERSION 3.11)

#  Curve-X Editor executable
project(curve-editor-x)
set(CMAKE_CXX_STANDARD 17)

#  Force Unicode
#  This step is required because of the Utils::get_user_open_file and Utils::get_user_open_files functions using wide strings
add_compile_definitions(UNICODE)
message("Forcing Unicode over Multi-byte (using Windows.h)")

#  List all .cpp files
file(GLOB_RECURSE curve-editor-x_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

#  Declare executable with sources, headers and linked libraries
add_executable(curve-editor-x "${curve-editor-x_SOURCES}")
target_include_directories(curve-editor-x PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/")
target_link_libraries(curve-editor-x PRIVATE curve-x raylib)

#  Execute other CMakeLists.txt
add_subdirectory("libs")

#  Additional include directories
# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/curve-x/include/")

#  List include directories
# message("Included directories: ${INCLUDE_DIRECTORIES}")
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()