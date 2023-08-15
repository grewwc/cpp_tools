#pragma once

#include <vector>
namespace wwc
{
    template <typename Container>
    void shell_sort(Container &vec)
    {
        int h = 1;
        const int n = vec.size();
        while (h < n / 3)
        {
            h = 3 * h + 1;
        }
        while (h >= 1)
        {
            for (int i = h; i < n; i++)
            {
                int j = i;
                for (; j > 0 && vec[j - h] > vec[i]; j -= h)
                    ;
                if (j < i)
                {
                    auto val_i = vec[i];
                    for (int k = i; k >= j + h; k -= h)
                    {
                        vec[k] = vec[k - h];
                    }
                    vec[j] = val_i;
                }
            }

            h /= 3;
        }
    }
} // namespace wwc