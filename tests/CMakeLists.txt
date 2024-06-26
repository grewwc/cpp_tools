# GoogleTest requires at least C++14

include(FetchContent)
message(STATUS ${CMAKE_SOURCE_DIR}/thirdparty/googletest)
FetchContent_Declare(
    googletest
    URL ${CMAKE_SOURCE_DIR}/thirdparty/googletest
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()

file(GLOB SOURCES "*.cc")

add_executable(
    all_test
    ${SOURCES}
)

include_directories(${CMAKE_SOURCE_DIR})
message(STATUS "here==> ${CMAKE_SOURCE_DIR}")
target_link_libraries(
    all_test
    GTest::gtest_main
    cpp_tools
)

include(GoogleTest)

gtest_discover_tests(all_test)
