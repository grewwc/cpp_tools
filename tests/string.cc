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
    if (vec[0] >= 5) {
        s = String("-") + s;
    }
    return s;
}

// shell_sort
TEST(STRING_TEST, string_mul) {
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        String s = generate_random_number();
        double res1 = s.to_double() * s.to_double();
        double res2 = (s * s).to_double();
        double err = std::abs(res1 - res2) / res1;
        ASSERT_LE(err, 1e-5);
    }
}
