#include "RandomUtils.hpp"
#include "Types.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
using namespace std;

template <typename T>
vector<T> gen_vec_by_heap(const vector<T> &data, __helper::_Heap<T> &h)
{
    for (const T &val : data)
    {
        h.insert(val);
    }
    vector<T> result;
    result.reserve(data.size());
    while (!h.is_empty())
    {
        result.emplace_back(h.remove().value());
    }
    return result;
}

// test minheap
TEST(TYPE_TEST, MinHeap)
{
    wwc::MinHeap<double> h{16};
    vector<double> vec = wwc::uniform_gen<double, 500>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    vec = gen_vec_by_heap<double>(vec, h);
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}

// test maxheap
TEST(TYPE_TEST, MaxHeap)
{
    wwc::MaxHeap<double> h;
    vector<double> vec = wwc::uniform_gen<double, 500>(0, 1);
    ASSERT_FALSE(is_sorted(vec.begin(), vec.end()));
    vec = gen_vec_by_heap(vec, h);
    reverse(vec.begin(), vec.end());
    ASSERT_TRUE(is_sorted(vec.begin(), vec.end()));
}
