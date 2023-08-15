#pragma once
#include "helper/sort.hpp"
#include <utility>
namespace wwc
{
    template <typename Container>
    void shell_sort(Container &vec)
    {
        size_t h = 1;
        const size_t n = vec.size();
        while (h < n / 3)
        {
            h = 3 * h + 1;
        }
        while (h >= 1)
        {
            for (size_t i = h; i < n; i++)
            {
                size_t j = i;
                for (; j > 0 && vec[j - h] > vec[i]; j -= h)
                    ;
                if (j < i)
                {
                    auto val_i = vec[i];
                    for (size_t k = i; k >= j + h; k -= h)
                    {
                        vec[k] = vec[k - h];
                    }
                    vec[j] = val_i;
                }
            }

            h /= 3;
        }
    }

    template <typename Container>
    void quick_sort(Container &vec)
    {
        ::__helper::quick_sort(vec, 0, vec.size());
    }

    template <typename Container>
    void insertion_sort(Container &vec)
    {
        ::__helper::insertion_sort(vec, 0, vec.size());
    }
} // namespace wwc
