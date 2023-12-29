#pragma once
#include "src/types.hpp"
#include <utility>

#include "src/types.hpp"
#include "src/hierachy_mutex.hpp"
#include "include/BS_thread_pool.hpp"
#include "src/stopwatch.hpp"

namespace wwc {
    template <typename T>
    class MinHeap : public __helper::_Heap<T> {
    public:
        explicit MinHeap(int sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T &val1, const T &val2) const override {
            return val1 < val2;
        }
    };

    template <typename T>
    class MaxHeap : public __helper::_Heap<T> {
    public:
        explicit MaxHeap(int sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T &val1, const T &val2) const override {
            return val1 > val2;
        }
    };
}  // namespace wwc
