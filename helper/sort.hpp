#pragma once
#include <utility>

#define INSERTION_SORT_THREASH 16

namespace __helper
{
    template <typename Container>
    void insertion_sort(Container &vec, size_t lo, size_t hi)
    {
        if (hi - lo <= 1)
        {
            return;
        }
        for (size_t i = lo + 1; i < hi; i++)
        {
            size_t j = i;
            for (; j > 0 && vec[i] < vec[j - 1]; j--)
                ;
            if (j != i)
            {
                auto val = vec[i];
                for (size_t k = i; k > j; k--)
                {
                    vec[k] = vec[k - 1];
                }
                vec[j] = val;
            }
        }
    }

    using interval_t = std::pair<size_t, size_t>;

    template <typename Container>
    interval_t _partition(Container &vec, size_t lo, size_t hi)
    {
        const auto pivot = vec[lo];
        size_t lt = lo;
        size_t i = lo;
        size_t gt = hi - 1;
        while (i <= gt)
        {
            if (vec[i] == pivot)
            {
                i++;
            }
            else if (vec[i] < pivot)
            {
                std::swap(vec[i++], vec[lt++]);
            }
            else
            {
                std::swap(vec[i], vec[gt--]);
            }
        }
        return std::make_pair(lt, gt);
    }

    template <typename Container>
    void quick_sort(Container &vec, size_t lo, size_t hi)
    {
        if (hi - lo < INSERTION_SORT_THREASH)
        {
            insertion_sort(vec, lo, hi);
            return;
        }
        auto p = _partition(vec, lo, hi);
        quick_sort(vec, lo, p.first);
        quick_sort(vec, p.second + 1, hi);
    }

} // namespace __helper