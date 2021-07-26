# cmake_minimum_required(VERSION <specify CMake version here>)
project(Codable)
project(JSON)

set(CMAKE_CXX_STANDARD 14)

add_library(Codable Codable.cpp Codable.h)
add_library(JSON JSON.cpp JSON.h)