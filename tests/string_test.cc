#include "src/String.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>

#include "RandomUtils.hpp"

using namespace wwc;
using namespace std;

String generate_random_number() {
    vector<int> vec = uniform_gen<int, 10>(0, 9);
    String s;
    s.reserve(vec.size());
    for (auto e : vec) {
        s += (char)(e + '0');
    }
    if (vec[0] >= 3) {
        s = String("-") + s;
    } else if (vec[0] < 7) {
        s = string("+") + s;
    }
    return s;
}

// string_mul
TEST(STRING_TEST, string_mul) {
    for (int i = 0; i < 100; i++) {
        String s1 = generate_random_number();
        String s2 = generate_random_number();
        double res1 = s1.to_double() * s2.to_double();
        double res2 = (s1 * s2).to_double();
        double err = std::abs(std::abs(res1 - res2) / res1);
        ASSERT_LE(err, 1e-5);
    }
}

// string_plus
TEST(STRING_TEST, string_plus) {
    for (int i = 0; i < 100; i++) {
        String s1 = generate_random_number();
        String s2 = generate_random_number();
        double res1 = s1.to_double() + s2.to_double();
        double res2 = (s1.plus(s2)).to_double();
        double err = std::abs(std::abs(res1 - res2) / res1);
#if 0
        std::cout << i << ", " << s1 << ": " << s2 << " , " << res1 << " , " << res2 << " , " << err << std::endl;
#endif
        ASSERT_LE(err, 1e-5);
    }
}
