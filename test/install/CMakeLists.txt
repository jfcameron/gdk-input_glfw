# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(gdk-input_glfw_consumer LANGUAGES CXX)

find_package(gdk-input_glfw 1 REQUIRED)

add_executable(consumer main.cpp)

target_link_libraries(consumer PRIVATE gdk::input_glfw)

set_target_properties(consumer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<0:>")
