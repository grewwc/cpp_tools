#include "Algorithm.hpp"
#include "RandomUtils.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace std;

// merge_sort
TEST(SEARCH_TEST, binary_search)
{
    vector<double> vec = wwc::uniform_gen<double, 500>(0, 1);
    wwc::merge_sort(vec);
    double target = vec[50];
    long index = wwc::binary_search(vec, target);
    ASSERT_TRUE(index==50);
}