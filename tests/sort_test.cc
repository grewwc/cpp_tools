#include "src/algorithm.hpp"
#include "RandomUtils.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>
using namespace std;

// shell_sort
TEST(SORT_TEST, shell_sort)
{
    vector<double> vec = wwc::uniform_gen<double, 50>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    wwc::shell_sort(vec);
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}

// quick_sort
TEST(SORT_TEST, quick_sort)
{
    vector<double> vec = wwc::uniform_gen<double, 50>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    wwc::quick_sort(vec);
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}

// insertion_sort
TEST(SORT_TEST, insertion_sort)
{
    vector<double> vec = wwc::uniform_gen<double, 50>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    wwc::insertion_sort(vec);
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}

// merge_sort
TEST(SORT_TEST, merge_sort)
{
    vector<double> vec = wwc::uniform_gen<double, 500>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    wwc::merge_sort(vec);
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}