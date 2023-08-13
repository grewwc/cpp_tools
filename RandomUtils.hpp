#pragma once
#include <algorithm>
#include <random>
#include <vector>
static std::random_device __device;

using namespace std;

namespace wwc
{
    template <typename T, unsigned N>
    std::vector<T> uniform_gen(T min, T max)
    {
        std::mt19937 __generator(__device());
        std::vector<T> result(N);

        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> dist(min, max);
            auto generator = [&]() { return dist(__generator); };
            std::generate_n(std::begin(result), N, generator);
        }
        else
        {
            std::uniform_real_distribution<T> dist(min, max);
            auto generator = [&]() { return dist(__generator); };
            std::generate_n(std::begin(result), N, generator);
        }
        return result;
    }
} // namespace wwc