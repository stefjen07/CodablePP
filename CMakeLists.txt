cmake_minimum_required(VERSION 3.0)
if(NOT TARGET Codable)
    project(Codable)

    set(CMAKE_CXX_STANDARD 14)

    add_library(Codable Codable.cpp Codable.h JSON.cpp JSON.h)

    #add_executable(test_codable main.cpp)
    #target_link_libraries(test_codable Codable)
endif()