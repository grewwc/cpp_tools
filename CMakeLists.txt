cmake_minimum_required(VERSION 3.14)

project("dev")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_definitions(-DGFLAGS_BAZEL_BUILD=true -DHAVE_PTHREAD=true)

add_library(cpp_tools STATIC "")

add_subdirectory(tests)
add_subdirectory(thirdparty)

message("Version=${CMAKE_CXX_STANDARD}")


