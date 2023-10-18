#pragma once
#include <iostream>
#include <utility>

#include "helper/sort.hpp"
namespace wwc {
    template <typename Container>
    void shell_sort(Container &vec) {
        size_t h = 1;
        const size_t n = vec.size();
        while (h < n / 3) {
            h = 3 * h + 1;
        }
        while (h >= 1) {
            for (size_t i = h; i < n; i++) {
                size_t j = i;
                for (; j > 0 && vec[j - h] > vec[i]; j -= h)
                    ;
                if (j < i) {
                    auto val_i = vec[i];
                    for (size_t k = i; k >= j + h; k -= h) {
                        vec[k] = vec[k - h];
                    }
                    vec[j] = val_i;
                }
            }

            h /= 3;
        }
    }

    template <typename Container>
    void quick_sort(Container &vec) {
        ::__helper::quick_sort(vec, 0, vec.size());
    }

    template <typename Container>
    void insertion_sort(Container &vec) {
        ::__helper::insertion_sort(vec, 0, vec.size());
    }

    template <template <typename, typename...> class Container, typename T>
    void merge_sort(Container<T> &vec) {
        if (vec.size() < INSERTION_SORT_THREASH) {
            ::__helper::insertion_sort(vec, 0, vec.size());
            return;
        }
        const size_t len = vec.size();
        const size_t half_len = len / 2 + 1;
        Container<T> aux;
        aux.reserve(vec.size());
        for (size_t sz = 1; sz < len; sz *= 2) {
            for (size_t i = 0; i + sz < len; i += sz * 2) {
                ::__helper::_merge(vec, i, i + sz, std::min(i + 2 * sz, len),
                                   aux);
            }
        }
    }

    template <template <typename, typename...> class Container, typename T>
    long binary_search_left(const Container<T> &container, const T &target,
                            long lo = 0, long hi = 0) {
        auto sz = container.size();
        if (sz == 0) {
            return -1;
        }
        if (hi == 0) {
            hi = static_cast<long>(sz);
        }
        while (lo < hi) {
            auto mid = (hi - lo) / 2 + lo;
            if (container[mid] < target) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        return lo;
    }

    template <template <typename, typename...> class Container, typename T>
    long binary_search_right(const Container<T> &container, const T &target) {
        const auto sz = container.size();
        size_t lo = 0, hi = sz;
        while (lo < hi) {
            auto mid = (hi - lo) / 2 + lo;
            if (container[mid] > target) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        return lo;
    }

}  // namespace wwc
