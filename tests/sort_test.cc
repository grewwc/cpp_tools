#include "RandomUtils.hpp"
#include "Algorithm.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>
using namespace std;

// Demonstrate some basic assertions.
TEST(HelloTest, shell_sort)
{
    vector<double> vec = wwc::uniform_gen<double, 50>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    wwc::shell_sort(vec);
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}