add_test([=[HelloTest.shell_sort]=]  /Users/wwc129/cpp_tools/build/tests/sort_test [==[--gtest_filter=HelloTest.shell_sort]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HelloTest.shell_sort]=]  PROPERTIES WORKING_DIRECTORY /Users/wwc129/cpp_tools/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  sort_test_TESTS HelloTest.shell_sort)
