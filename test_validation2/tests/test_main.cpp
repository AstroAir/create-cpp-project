#include <gtest/gtest.h>

// Simple test case example
TEST(ExampleTest, SimpleTest) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

// Parameterized test example
class ParameterizedTest : public ::testing::TestWithParam<int> {};

TEST_P(ParameterizedTest, IsPositive) {
    EXPECT_GT(GetParam(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    PositiveValues,
    ParameterizedTest,
    ::testing::Values(1, 2, 3, 5, 8)
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
