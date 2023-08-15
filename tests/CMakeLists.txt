# GoogleTest requires at least C++14

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
    sort_test
    sort_test.cc
)

target_link_libraries(
    sort_test
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(sort_test)
