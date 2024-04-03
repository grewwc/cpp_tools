#include <gtest/gtest.h>
#include "src/num_utils.hpp"

TEST(NUM_UTILS_TEST, round_int_test)
{
    ASSERT_EQ(wwc::round(1L), 1);
    ASSERT_EQ(wwc::round(13L), 10);
    ASSERT_EQ(wwc::round(5L), 10);
    ASSERT_EQ(wwc::round(15L), 20);
    ASSERT_EQ(wwc::round(-1103L), -1100);
    ASSERT_EQ(wwc::round(-1105L), -1110);
    ASSERT_EQ(wwc::round(0L), 0);
    ASSERT_EQ(wwc::round(37880L), 37900);
}

TEST(NUM_UTILS_TEST, round_float_test)
{
    // floating point 
    ASSERT_EQ(wwc::round(1.5), 2);
    ASSERT_EQ(wwc::round(1.50), 2);
    ASSERT_EQ(wwc::round(1.400), 1);
    ASSERT_EQ(wwc::round(0.400), 0);
    ASSERT_EQ(wwc::round(-0.40), 0);
    ASSERT_EQ(wwc::round(-0.440), -0.4);
    ASSERT_EQ(wwc::round(-0.445), -0.45);
    ASSERT_EQ(wwc::round(5.0), 10.0);
}