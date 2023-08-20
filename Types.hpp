#pragma once
#include "helper/types.hpp"
#include <utility>

namespace wwc
{
    template <typename T>
    class MinHeap : public __helper::_Heap<T>
    {
    public:
        explicit MinHeap(int sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T &val1, const T &val2) const { return val1 < val2; }
    };

    template <typename T>
    class MaxHeap : public __helper::_Heap<T>
    {
    public:
        explicit MaxHeap(int sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T &val1, const T &val2) const { return val1 > val2; }
    };
} // namespace wwc
