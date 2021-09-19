cmake_minimum_required(VERSION 3.0)
if(NOT TARGET Codable)
    project(Codable)
    include(CTest)

    set(CMAKE_CXX_STANDARD 11)

    include_directories(include)

    add_library(Codable include/Codable.hpp src/JSON.cpp include/JSON.hpp)

    if(BUILD_TESTING)
        add_executable(test_codable test/test.cpp)
        target_link_libraries(test_codable Codable)
        add_test(Codable test_codable)
    endif()

    install(TARGETS Codable DESTINATION lib)
endif()
