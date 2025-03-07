cmake_minimum_required(VERSION 3.10) # Or higher

project(libtinybvh)

add_library(libtinybvh INTERFACE)
target_include_directories(libtinybvh INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})