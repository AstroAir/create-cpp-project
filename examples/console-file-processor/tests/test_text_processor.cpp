/**
 * @file test_text_processor.cpp
 * @brief Tests for TextProcessor class
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include "../src/core/text_processor.h"

TEST(TextProcessorTest, CountWords) {
    EXPECT_EQ(core::TextProcessor::count_words(""), 0);
    EXPECT_EQ(core::TextProcessor::count_words("hello"), 1);
    EXPECT_EQ(core::TextProcessor::count_words("hello world"), 2);
    EXPECT_EQ(core::TextProcessor::count_words("  hello   world  "), 2);
}

TEST(TextProcessorTest, CountLines) {
    EXPECT_EQ(core::TextProcessor::count_lines(""), 0);
    EXPECT_EQ(core::TextProcessor::count_lines("hello"), 1);
    EXPECT_EQ(core::TextProcessor::count_lines("hello\nworld"), 2);
    EXPECT_EQ(core::TextProcessor::count_lines("hello\nworld\n"), 3);
}

TEST(TextProcessorTest, CountCharacters) {
    EXPECT_EQ(core::TextProcessor::count_characters(""), 0);
    EXPECT_EQ(core::TextProcessor::count_characters("hello"), 5);
    EXPECT_EQ(core::TextProcessor::count_characters("hello world"), 11);
}

TEST(TextProcessorTest, SearchPattern) {
    std::string text = "Hello world, hello universe";
    auto matches = core::TextProcessor::search_pattern(text, "hello", false);
    EXPECT_EQ(matches.size(), 2);
    
    matches = core::TextProcessor::search_pattern(text, "hello", true);
    EXPECT_EQ(matches.size(), 1);
}

TEST(TextProcessorTest, ReplacePattern) {
    std::string text = "Hello world";
    std::string result = core::TextProcessor::replace_pattern(text, "world", "universe");
    EXPECT_EQ(result, "Hello universe");
}
