#include <gtest/gtest.h>

TEST(init, test) {
    constexpr int lhs = 1;
    constexpr int rhs = 2;

    ASSERT_EQ(lhs + rhs, 3);
}