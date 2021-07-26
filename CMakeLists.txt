# cmake_minimum_required(VERSION <specify CMake version here>)
if (NOT TARGET Codable)
project(Codable)
endif()

if (NOT TARGET JSON)
project(JSON)
endif()

set(CMAKE_CXX_STANDARD 14)

add_library(Codable Codable.cpp Codable.h)
add_library(JSON JSON.cpp JSON.h)