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
                for (int j = i; j > 0; j -= h)
                {
                    if (vec[j] < vec[j - h])
                    {
                        swap(vec[j], vec[j - h]);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            h /= 3;
        }
    }
} // namespace wwc