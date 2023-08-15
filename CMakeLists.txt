cmake_minimum_required(VERSION 3.14)

project("dev")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(tests)

message("Version=${CMAKE_CXX_STANDARD}")
