#include <gtest/gtest.h>
#include "utils/core/string_utils.h"

using namespace utils;

class StringUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(StringUtilsTest, ToLower) {
    EXPECT_EQ("hello world", StringUtils::toLower("HELLO WORLD"));
    EXPECT_EQ("hello world", StringUtils::toLower("Hello World"));
    EXPECT_EQ("hello world", StringUtils::toLower("hello world"));
    EXPECT_EQ("", StringUtils::toLower(""));
    EXPECT_EQ("123", StringUtils::toLower("123"));
    EXPECT_EQ("hello123world", StringUtils::toLower("HELLO123WORLD"));
}

TEST_F(StringUtilsTest, ToUpper) {
    EXPECT_EQ("HELLO WORLD", StringUtils::toUpper("hello world"));
    EXPECT_EQ("HELLO WORLD", StringUtils::toUpper("Hello World"));
    EXPECT_EQ("HELLO WORLD", StringUtils::toUpper("HELLO WORLD"));
    EXPECT_EQ("", StringUtils::toUpper(""));
    EXPECT_EQ("123", StringUtils::toUpper("123"));
    EXPECT_EQ("HELLO123WORLD", StringUtils::toUpper("hello123world"));
}

TEST_F(StringUtilsTest, Replace) {
    EXPECT_EQ("Hello C++", StringUtils::replace("Hello World", "World", "C++"));
    EXPECT_EQ("C++ C++ C++", StringUtils::replace("foo foo foo", "foo", "C++"));
    EXPECT_EQ("Hello World", StringUtils::replace("Hello World", "xyz", "abc"));
    EXPECT_EQ("", StringUtils::replace("", "foo", "bar"));
    EXPECT_EQ("Hello World", StringUtils::replace("Hello World", "", "xyz"));
    EXPECT_EQ("abcdefabc", StringUtils::replace("123def123", "123", "abc"));
}

TEST_F(StringUtilsTest, Split) {
    auto result = StringUtils::split("hello,world,cpp", ',');
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("hello", result[0]);
    EXPECT_EQ("world", result[1]);
    EXPECT_EQ("cpp", result[2]);
    
    result = StringUtils::split("one", ',');
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("one", result[0]);
    
    result = StringUtils::split("", ',');
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("", result[0]);
    
    result = StringUtils::split("a,b,c,", ',');
    ASSERT_EQ(4, result.size());
    EXPECT_EQ("a", result[0]);
    EXPECT_EQ("b", result[1]);
    EXPECT_EQ("c", result[2]);
    EXPECT_EQ("", result[3]);
}

TEST_F(StringUtilsTest, SplitWithDifferentDelimiters) {
    auto result = StringUtils::split("hello world cpp", ' ');
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("hello", result[0]);
    EXPECT_EQ("world", result[1]);
    EXPECT_EQ("cpp", result[2]);
    
    result = StringUtils::split("path/to/file", '/');
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("path", result[0]);
    EXPECT_EQ("to", result[1]);
    EXPECT_EQ("file", result[2]);
}

TEST_F(StringUtilsTest, Trim) {
    EXPECT_EQ("hello world", StringUtils::trim("  hello world  "));
    EXPECT_EQ("hello world", StringUtils::trim("hello world"));
    EXPECT_EQ("hello world", StringUtils::trim("\t\nhello world\t\n"));
    EXPECT_EQ("", StringUtils::trim("   "));
    EXPECT_EQ("", StringUtils::trim(""));
    EXPECT_EQ("a", StringUtils::trim("  a  "));
    EXPECT_EQ("hello\nworld", StringUtils::trim("  hello\nworld  "));
}

TEST_F(StringUtilsTest, StartsWith) {
    EXPECT_TRUE(StringUtils::startsWith("hello world", "hello"));
    EXPECT_TRUE(StringUtils::startsWith("hello world", ""));
    EXPECT_TRUE(StringUtils::startsWith("hello", "hello"));
    EXPECT_FALSE(StringUtils::startsWith("hello world", "world"));
    EXPECT_FALSE(StringUtils::startsWith("hello", "hello world"));
    EXPECT_FALSE(StringUtils::startsWith("", "hello"));
    EXPECT_TRUE(StringUtils::startsWith("", ""));
}

TEST_F(StringUtilsTest, EndsWith) {
    EXPECT_TRUE(StringUtils::endsWith("hello world", "world"));
    EXPECT_TRUE(StringUtils::endsWith("hello world", ""));
    EXPECT_TRUE(StringUtils::endsWith("world", "world"));
    EXPECT_FALSE(StringUtils::endsWith("hello world", "hello"));
    EXPECT_FALSE(StringUtils::endsWith("world", "hello world"));
    EXPECT_FALSE(StringUtils::endsWith("", "world"));
    EXPECT_TRUE(StringUtils::endsWith("", ""));
}

TEST_F(StringUtilsTest, StartsWithCaseSensitive) {
    EXPECT_TRUE(StringUtils::startsWith("Hello World", "Hello"));
    EXPECT_FALSE(StringUtils::startsWith("Hello World", "hello"));
    EXPECT_FALSE(StringUtils::startsWith("hello world", "Hello"));
}

TEST_F(StringUtilsTest, EndsWithCaseSensitive) {
    EXPECT_TRUE(StringUtils::endsWith("Hello World", "World"));
    EXPECT_FALSE(StringUtils::endsWith("Hello World", "world"));
    EXPECT_FALSE(StringUtils::endsWith("hello world", "World"));
}

TEST_F(StringUtilsTest, ComplexOperations) {
    std::string input = "  Hello, Beautiful World!  ";
    std::string result = StringUtils::trim(input);
    result = StringUtils::replace(result, "Beautiful", "Amazing");
    result = StringUtils::toLower(result);
    
    EXPECT_EQ("hello, amazing world!", result);
}

TEST_F(StringUtilsTest, EdgeCases) {
    // Test with special characters (ASCII only - Unicode not supported in basic implementation)
    // Note: Current implementation only handles ASCII characters
    std::string withAccents = "HÉLLO WÖRLD";
    std::string lowerResult = StringUtils::toLower(withAccents);
    // Just verify it doesn't crash and returns something
    EXPECT_FALSE(lowerResult.empty());

    // Test with numbers and symbols
    std::string mixed = "Hello123!@#World";
    EXPECT_EQ("hello123!@#world", StringUtils::toLower(mixed));
    EXPECT_EQ("HELLO123!@#WORLD", StringUtils::toUpper(mixed));

    // Test replace with multiple occurrences (each occurrence gets replaced)
    EXPECT_EQ("aabbccaabbccaabbcc", StringUtils::replace("abcabcabc", "abc", "aabbcc"));
}

TEST_F(StringUtilsTest, UnicodeHandling) {
    // Basic Unicode test (may depend on locale)
    std::string unicode = "Hello 世界";
    EXPECT_FALSE(StringUtils::toLower(unicode).empty());
    EXPECT_FALSE(StringUtils::toUpper(unicode).empty());
    
    // Test that Unicode characters don't break the functions
    EXPECT_TRUE(StringUtils::startsWith("Hello 世界", "Hello"));
    EXPECT_TRUE(StringUtils::endsWith("Hello 世界", "世界"));
}
