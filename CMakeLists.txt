cmake_minimum_required(VERSION 3.0)
if(NOT TARGET Codable)
project(Codable)
project(JSON)

set(CMAKE_CXX_STANDARD 14)

add_library(Codable Codable.cpp Codable.h)
add_library(JSON JSON.cpp JSON.h)
endif()