#include <gtest/gtest.h>
// #include "Sample.h"

class SampleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures here
    }

    void TearDown() override {
        // Clean up after tests
    }

    // Test fixture members
};

// Basic test
TEST_F(SampleTest, BasicTest) {
    EXPECT_TRUE(true);
    ASSERT_FALSE(false);
}

// Parameterized test example
class SampleParameterizedTest : public ::testing::TestWithParam<int> {
};

TEST_P(SampleParameterizedTest, ParameterTest) {
    int value = GetParam();
    EXPECT_GE(value, 0);
}

INSTANTIATE_TEST_SUITE_P(
    SampleValues,
    SampleParameterizedTest,
    ::testing::Values(1, 2, 3, 4, 5)
);

// Death test example
TEST(SampleDeathTest, CrashTest) {
    // EXPECT_DEATH(SomeFunction(), "Expected error message");
}
