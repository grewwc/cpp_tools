# Create test executable
add_executable(runUnitTests test_strings.cpp)

# Link against Catch2's main-providing target
target_link_libraries(runUnitTests PRIVATE Catch2::Catch2WithMain)

# Register test with CTest
add_test(NAME UnitTests COMMAND runUnitTests)
