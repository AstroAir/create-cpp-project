#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "utils/ui/terminal_utils.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#endif

using namespace utils;

class UTF8EncodingTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Initialize UTF-8 support before each test
        TerminalUtils::initializeUtf8Support();
    }

    void TearDown() override {
        // Clean up after each test if needed
    }
};

TEST_F(UTF8EncodingTest, InitializeUtf8Support) {
    bool result = TerminalUtils::initializeUtf8Support();
    EXPECT_TRUE(result);

#ifdef _WIN32
    // Verify that code pages are set correctly
    UINT outputCP = GetConsoleOutputCP();
    UINT inputCP = GetConsoleCP();

    EXPECT_EQ(outputCP, CP_UTF8) << "Output code page should be UTF-8 (65001)";
    EXPECT_EQ(inputCP, CP_UTF8) << "Input code page should be UTF-8 (65001)";
#endif
}

TEST_F(UTF8EncodingTest, TestUtf8Encoding) {
    bool result = TerminalUtils::testUtf8Encoding();
    EXPECT_TRUE(result) << "UTF-8 encoding test should pass";
}

TEST_F(UTF8EncodingTest, ChineseCharacterSupport) {
    // Test Chinese characters
    std::string chinese = "你好世界！欢迎使�?C++ 项目脚手�?;

    // These should not throw exceptions or cause crashes
    EXPECT_NO_THROW({ std::cout << chinese << std::endl; });

    // Verify the string is not empty and contains expected bytes
    EXPECT_FALSE(chinese.empty());
    EXPECT_GT(chinese.length(), 10);  // Chinese characters are multi-byte
}

TEST_F(UTF8EncodingTest, EmojiSupport) {
    // Test various emojis
    std::string emojis = "🚀 🌟 💻 🔧 �?🎯 📦 🛠�?🌈 🎉";

    EXPECT_NO_THROW({ std::cout << emojis << std::endl; });

    EXPECT_FALSE(emojis.empty());
    EXPECT_GT(emojis.length(), 20);  // Emojis are multi-byte
}

TEST_F(UTF8EncodingTest, InternationalCharacters) {
    // Test various international characters
    std::vector<std::string> international_texts = {
            "こんにちは世界！",   // Japanese
            "안녕하세�?세계!",   // Korean
            "مرحبا بالعالم!",     // Arabic
            "שלום עולם!",         // Hebrew
            "Привет мир!",        // Russian
            "Γεια σας κόσμε!",    // Greek
            "Café naïve résumé",  // Accented Latin
    };

    for (const auto& text : international_texts) {
        EXPECT_NO_THROW({ std::cout << text << std::endl; });
        EXPECT_FALSE(text.empty());
    }
}

TEST_F(UTF8EncodingTest, MathematicalSymbols) {
    std::string math_symbols = "�?�?�?�?�?�?�?�?± ÷ × π α β γ δ ε";

    EXPECT_NO_THROW({ std::cout << math_symbols << std::endl; });

    EXPECT_FALSE(math_symbols.empty());
}

TEST_F(UTF8EncodingTest, BoxDrawingCharacters) {
    std::string box_chars = "┌─┬─�?�?�?�?├─┼─�?�?�?�?└─┴─�?;

    EXPECT_NO_THROW({ std::cout << box_chars << std::endl; });

    EXPECT_FALSE(box_chars.empty());
}

TEST_F(UTF8EncodingTest, CurrencySymbols) {
    std::string currency = "$ �?£ ¥ �?�?�?�?�?�?�?�?�?�?;

    EXPECT_NO_THROW({ std::cout << currency << std::endl; });

    EXPECT_FALSE(currency.empty());
}

TEST_F(UTF8EncodingTest, ColorizedUnicodeOutput) {
    if (TerminalUtils::supportsAnsi()) {
        std::string red_chinese = TerminalUtils::colorize("红色中文", Color::Red);
        std::string green_emoji = TerminalUtils::colorize("🟢 绿色", Color::Green);

        EXPECT_NO_THROW({
            std::cout << red_chinese << std::endl;
            std::cout << green_emoji << std::endl;
        });

        EXPECT_FALSE(red_chinese.empty());
        EXPECT_FALSE(green_emoji.empty());
    }
}

TEST_F(UTF8EncodingTest, ComplexUnicodeCombinations) {
    std::string complex = "🚀 C++ 项目脚手�?🛠�?Project Scaffolding Tool 🎯";

    EXPECT_NO_THROW({ std::cout << complex << std::endl; });

    EXPECT_FALSE(complex.empty());
    EXPECT_GT(complex.length(), 30);
}

TEST_F(UTF8EncodingTest, TerminalUtilsWithUnicode) {
    // Test terminal utilities with Unicode content
    std::vector<std::string> unicode_lines = {"🚀 第一行：项目创建", "📦 第二行：包管�?,
                                              "🔧 第三行：构建配置"};

    EXPECT_NO_THROW({ TerminalUtils::showBox(unicode_lines, BorderStyle::Single); });

    EXPECT_NO_THROW({ TerminalUtils::showCard("Unicode 测试卡片", unicode_lines); });
}

TEST_F(UTF8EncodingTest, LoggingWithUnicode) {
    // Test that our logging system can handle Unicode
    // This simulates what happens in the main application
    std::string project_name = "测试项目";
    std::string log_message = "🚀 Creating console project: " + project_name;

    EXPECT_NO_THROW({ std::cout << log_message << std::endl; });

    EXPECT_FALSE(log_message.empty());
    EXPECT_NE(log_message.find("🚀"), std::string::npos);
    EXPECT_NE(log_message.find("测试项目"), std::string::npos);
}

TEST_F(UTF8EncodingTest, ProgressIndicatorWithUnicode) {
    // Test progress indicators with Unicode
    std::string unicode_task = "正在创建项目结构...";

    EXPECT_NO_THROW({
        std::cout << "�?" << unicode_task << std::endl;
        std::cout << "�?高性能处理�?.." << std::endl;
        std::cout << "🎉 完成�? << std::endl;
    });
}
