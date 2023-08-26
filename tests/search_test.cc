#include "Algorithm.hpp"
#include "RandomUtils.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace std;

// binary_search_left
TEST(SEARCH_TEST, binary_search_left)
{
    vector<double> vec = wwc::uniform_gen<double, 500>(0, 1);
    wwc::merge_sort(vec);
    double target = vec[50];
    long index = wwc::binary_search_left(vec, target);
    ASSERT_TRUE(index==50);
}

// binary_serch_right
TEST(SEARCH_TEST, binary_search_right)
{
    vector<double> vec = wwc::uniform_gen<double, 500>(0, 1);
    wwc::merge_sort(vec);
    double target = vec[50];
    long index = wwc::binary_search_right(vec, target);
    ASSERT_TRUE(index==51);
}