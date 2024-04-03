#include "num_utils.hpp"
#include <cmath>

constexpr double eps = 1e-8;

static bool eq(double a, double b) {
    return std::abs(a - b) < eps;
}

static int get_num_of_floating_point_digit(double val) {
    int cnt = 0;
    while (!eq(std::round(val), val)) {
        cnt++;
        val *= 10;
    }
    return cnt;
}

static int get_num_of_digit(long val) {
    if (val == 0) {
        return 1;
    }
    int cnt = 0;
    while (val > 0) {
        cnt++;
        val /= 10;
    }
    return cnt;
}

namespace wwc {
    long round(long val, int significant_digit_num) {
        if (val == 0 || significant_digit_num == 0) {
            return 0;
        }
        const int num_of_digit = get_num_of_digit(val);
        if (num_of_digit == significant_digit_num) {
            return val;
        }
        if (significant_digit_num == -1) {
            int val_copy = val;
            while (val > 0 && val_copy % 10 == 0) {
                val_copy /= 10;
            }
            significant_digit_num = get_num_of_digit(val_copy) - 1;
        }
        const double scale = std::pow(10, num_of_digit - significant_digit_num);
        int result = static_cast<int>(std::round(val / scale) * scale);
        return result == 0 ? val : result;
    }

    double round(double val) {
        if (eq(val, 0.0)) {
            return 0.0;
        }
        const int num_digit = get_num_of_floating_point_digit(val);
        const double scale = std::pow(10, num_digit - 1);
        return std::round(val * scale) / scale;
    }
} // namespace wwc