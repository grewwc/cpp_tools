#pragma once
#include <utility>

#include "include/BS_thread_pool.hpp"
#include "src/files.hpp"
#include "src/hierachy_mutex.hpp"
#include "src/stopwatch.hpp"
#include "src/types.hpp"
#include "src/uf.hpp"
namespace wwc {
    template <typename T>
    class MinHeap : public __helper::_Heap<T> {
    public:
        explicit MinHeap(std::size_t sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T &val1, const T &val2) const override { return val1 < val2; }
    };

    template <typename T>
    class MaxHeap : public __helper::_Heap<T> {
    public:
        explicit MaxHeap(std::size_t sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T &val1, const T &val2) const override { return val1 > val2; }
    };
}  // namespace wwc
