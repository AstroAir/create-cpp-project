#include <gtest/gtest.h>
#include "../src/utils/terminal_utils.h"
#include <sstream>

using namespace utils;

class TerminalUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TerminalUtilsTest, SupportsAnsi) {
    // This test just ensures the function doesn't crash
    // The actual result depends on the terminal
    bool result = TerminalUtils::supportsAnsi();
    EXPECT_TRUE(result == true || result == false); // Should be boolean
}

TEST_F(TerminalUtilsTest, ColorizeText) {
    std::string text = "Hello World";
    std::string colorized = TerminalUtils::colorize(text, Color::Red);
    
    // If ANSI is supported, the string should be longer (contains escape codes)
    // If not supported, it should be the same
    if (TerminalUtils::supportsAnsi()) {
        EXPECT_GT(colorized.length(), text.length());
    } else {
        EXPECT_EQ(colorized, text);
    }
}

TEST_F(TerminalUtilsTest, ColorizeWithBackground) {
    std::string text = "Hello World";
    std::string colorized = TerminalUtils::colorize(text, Color::Red, Color::Blue);
    
    if (TerminalUtils::supportsAnsi()) {
        EXPECT_GT(colorized.length(), text.length());
        // Should contain both foreground and background codes
    } else {
        EXPECT_EQ(colorized, text);
    }
}

TEST_F(TerminalUtilsTest, StylizeText) {
    std::string text = "Hello World";
    std::vector<TextStyle> styles = {TextStyle::Bold, TextStyle::Italic};
    std::string stylized = TerminalUtils::stylize(text, styles);
    
    if (TerminalUtils::supportsAnsi()) {
        EXPECT_GT(stylized.length(), text.length());
    } else {
        EXPECT_EQ(stylized, text);
    }
}

TEST_F(TerminalUtilsTest, ColorAndStyle) {
    std::string text = "Hello World";
    std::vector<TextStyle> styles = {TextStyle::Bold};
    std::string result = TerminalUtils::colorAndStyle(text, Color::Green, styles, Color::Black);
    
    if (TerminalUtils::supportsAnsi()) {
        EXPECT_GT(result.length(), text.length());
    } else {
        EXPECT_EQ(result, text);
    }
}

TEST_F(TerminalUtilsTest, EmptyText) {
    std::string empty = "";
    std::string colorized = TerminalUtils::colorize(empty, Color::Red);
    
    if (TerminalUtils::supportsAnsi()) {
        // Should still contain escape codes even for empty text
        EXPECT_GT(colorized.length(), 0);
    } else {
        EXPECT_EQ(colorized, empty);
    }
}

TEST_F(TerminalUtilsTest, WrapText) {
    std::string longText = "This is a very long line of text that should be wrapped at a certain width to test the text wrapping functionality.";
    
    auto wrapped = TerminalUtils::wrapText(longText, 20);
    EXPECT_GT(wrapped.size(), 1); // Should be split into multiple lines
    
    // Each line (except possibly the last) should be <= 20 characters
    for (size_t i = 0; i < wrapped.size() - 1; ++i) {
        EXPECT_LE(wrapped[i].length(), 20);
    }
}

TEST_F(TerminalUtilsTest, WrapTextZeroWidth) {
    std::string text = "Hello World";
    auto wrapped = TerminalUtils::wrapText(text, 0);
    
    // With zero width, should return the original text as single line
    EXPECT_EQ(wrapped.size(), 1);
    EXPECT_EQ(wrapped[0], text);
}

TEST_F(TerminalUtilsTest, WrapTextShortWidth) {
    std::string text = "Hello";
    auto wrapped = TerminalUtils::wrapText(text, 3);
    
    // Should handle short widths gracefully
    EXPECT_GE(wrapped.size(), 1);
}

TEST_F(TerminalUtilsTest, GetBorderChars) {
    auto singleBorder = TerminalUtils::getBorderChars(BorderStyle::Single);
    EXPECT_EQ(singleBorder.size(), 11); // Should have 11 border characters (corners, edges, junctions)

    auto doubleBorder = TerminalUtils::getBorderChars(BorderStyle::Double);
    EXPECT_EQ(doubleBorder.size(), 11);

    auto roundedBorder = TerminalUtils::getBorderChars(BorderStyle::Rounded);
    EXPECT_EQ(roundedBorder.size(), 11);
}

TEST_F(TerminalUtilsTest, ShowInfoMessage) {
    // Test that info messages don't crash
    // We can't easily test the output without redirecting stdout
    EXPECT_NO_THROW(TerminalUtils::showInfo("Test info message"));
}

TEST_F(TerminalUtilsTest, ShowWarningMessage) {
    EXPECT_NO_THROW(TerminalUtils::showWarning("Test warning message"));
}

TEST_F(TerminalUtilsTest, ShowErrorMessage) {
    EXPECT_NO_THROW(TerminalUtils::showError("Test error message"));
}

TEST_F(TerminalUtilsTest, ShowSuccessMessage) {
    EXPECT_NO_THROW(TerminalUtils::showSuccess("Test success message"));
}

TEST_F(TerminalUtilsTest, ShowDebugMessage) {
    EXPECT_NO_THROW(TerminalUtils::showDebug("Test debug message"));
}

TEST_F(TerminalUtilsTest, ShowProgressBar) {
    // Test that progress bars don't crash
    EXPECT_NO_THROW(TerminalUtils::showProgressBar(50, 20));
    EXPECT_NO_THROW(TerminalUtils::showProgressBar(0, 20));
    EXPECT_NO_THROW(TerminalUtils::showProgressBar(100, 20));
}

TEST_F(TerminalUtilsTest, ShowProgressBarWithColors) {
    EXPECT_NO_THROW(TerminalUtils::showProgressBar(75, 30, Color::Green, Color::Red));
}

TEST_F(TerminalUtilsTest, ShowModernProgressBar) {
    EXPECT_NO_THROW(TerminalUtils::showModernProgressBar(60, "Loading..."));
    EXPECT_NO_THROW(TerminalUtils::showModernProgressBar(100, "Complete!"));
}

TEST_F(TerminalUtilsTest, ShowBox) {
    std::vector<std::string> lines = {"Line 1", "Line 2", "Line 3"};
    // Use the overload with explicit parameters to avoid ambiguity
    EXPECT_NO_THROW(TerminalUtils::showBox(lines, BorderStyle::Single, Color::Reset, Color::Reset, ""));
}

TEST_F(TerminalUtilsTest, ShowBoxWithStyle) {
    std::vector<std::string> lines = {"Styled Box", "With custom colors"};
    EXPECT_NO_THROW(TerminalUtils::showBox(lines, BorderStyle::Double, Color::Blue, Color::White, "Title"));
}

TEST_F(TerminalUtilsTest, ShowCard) {
    std::vector<std::string> content = {"Card content line 1", "Card content line 2"};
    EXPECT_NO_THROW(TerminalUtils::showCard("Card Title", content));
}

TEST_F(TerminalUtilsTest, ShowTwoColumnLayout) {
    std::vector<std::pair<std::string, std::string>> items = {
        {"Key 1", "Value 1"},
        {"Key 2", "Value 2"},
        {"Longer Key", "Longer Value"}
    };
    EXPECT_NO_THROW(TerminalUtils::showTwoColumnLayout(items));
}

TEST_F(TerminalUtilsTest, ShowFeatureList) {
    std::vector<std::pair<std::string, std::string>> features = {
        {"Feature 1", "Description of feature 1"},
        {"Feature 2", "Description of feature 2"}
    };
    EXPECT_NO_THROW(TerminalUtils::showFeatureList(features));
}

TEST_F(TerminalUtilsTest, ColorEnumValues) {
    // Test that all color enum values are valid
    std::vector<Color> colors = {
        Color::Reset, Color::Black, Color::Red, Color::Green,
        Color::Yellow, Color::Blue, Color::Magenta, Color::Cyan,
        Color::White, Color::BrightBlack, Color::BrightRed,
        Color::BrightGreen, Color::BrightYellow, Color::BrightBlue,
        Color::BrightMagenta, Color::BrightCyan, Color::BrightWhite
    };
    
    for (Color color : colors) {
        EXPECT_NO_THROW(TerminalUtils::colorize("test", color));
    }
}

TEST_F(TerminalUtilsTest, TextStyleEnumValues) {
    std::vector<TextStyle> styles = {
        TextStyle::Bold, TextStyle::Dim, TextStyle::Italic,
        TextStyle::Underline, TextStyle::Blink, TextStyle::Reverse,
        TextStyle::Hidden, TextStyle::Strike
    };

    for (TextStyle style : styles) {
        EXPECT_NO_THROW(TerminalUtils::stylize("test", {style}));
    }
}

TEST_F(TerminalUtilsTest, BorderStyleEnumValues) {
    std::vector<BorderStyle> styles = {
        BorderStyle::Single, BorderStyle::Double, BorderStyle::Rounded,
        BorderStyle::Bold, BorderStyle::Ascii
    };

    for (BorderStyle style : styles) {
        auto chars = TerminalUtils::getBorderChars(style);
        EXPECT_EQ(chars.size(), 11);
    }
}

TEST_F(TerminalUtilsTest, LongTextHandling) {
    std::string veryLongText(1000, 'A'); // 1000 'A' characters
    EXPECT_NO_THROW(TerminalUtils::colorize(veryLongText, Color::Red));
    
    auto wrapped = TerminalUtils::wrapText(veryLongText, 50);
    EXPECT_GT(wrapped.size(), 10); // Should be wrapped into many lines
}

TEST_F(TerminalUtilsTest, SpecialCharacters) {
    std::string specialText = "Special chars: !@#$%^&*()_+-=[]{}|;':\",./<>?";
    EXPECT_NO_THROW(TerminalUtils::colorize(specialText, Color::Green));
    
    auto wrapped = TerminalUtils::wrapText(specialText, 20);
    EXPECT_GE(wrapped.size(), 1);
}

TEST_F(TerminalUtilsTest, UnicodeText) {
    std::string unicodeText = "Unicode: 你好世界 🌍 ñáéíóú";
    EXPECT_NO_THROW(TerminalUtils::colorize(unicodeText, Color::Blue));
    
    auto wrapped = TerminalUtils::wrapText(unicodeText, 10);
    EXPECT_GE(wrapped.size(), 1);
}
