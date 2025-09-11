#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "src/utils/terminal_utils.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <clocale>
#include <locale>
#endif

using namespace utils;

class EncodingTester {
   public:
    struct TestCase {
        std::string name;
        std::string text;
        std::string description;
    };

    static void runAllTests() {
        std::cout << "=== UTF-8 Encoding Test Suite ===" << std::endl;
        std::cout << std::endl;

        // Initialize UTF-8 support
        std::cout << "1. Initializing UTF-8 support..." << std::endl;
        bool utf8Init = TerminalUtils::initializeUtf8Support();
        std::cout << "   UTF-8 initialization: " << (utf8Init ? "SUCCESS" : "FAILED") << std::endl;

        // Test ANSI support
        std::cout << "2. Testing ANSI support..." << std::endl;
        bool ansiSupport = TerminalUtils::supportsAnsi();
        std::cout << "   ANSI support: " << (ansiSupport ? "ENABLED" : "DISABLED") << std::endl;

        // Test UTF-8 encoding
        std::cout << "3. Testing UTF-8 encoding..." << std::endl;
        bool utf8Test = TerminalUtils::testUtf8Encoding();
        std::cout << "   UTF-8 test: " << (utf8Test ? "PASSED" : "FAILED") << std::endl;

        std::cout << std::endl;

        // Display current code page (Windows only)
#ifdef _WIN32
        displayCodePageInfo();
#else
        displayLocaleInfo();
#endif

        std::cout << std::endl;

        // Run character display tests
        runCharacterTests();

        std::cout << std::endl;

        // Test colored output with Unicode
        testColoredUnicode();

        std::cout << std::endl;
        std::cout << "=== Test Complete ===" << std::endl;
    }

   private:
    static void displayCodePageInfo() {
#ifdef _WIN32
        std::cout << "4. Windows Code Page Information:" << std::endl;
        UINT outputCP = GetConsoleOutputCP();
        UINT inputCP = GetConsoleCP();
        std::cout << "   Output Code Page: " << outputCP;
        if (outputCP == CP_UTF8) {
            std::cout << " (UTF-8)";
        } else if (outputCP == 936) {
            std::cout << " (GBK/GB2312 - Chinese Simplified)";
        } else if (outputCP == 950) {
            std::cout << " (Big5 - Chinese Traditional)";
        }
        std::cout << std::endl;

        std::cout << "   Input Code Page: " << inputCP;
        if (inputCP == CP_UTF8) {
            std::cout << " (UTF-8)";
        } else if (inputCP == 936) {
            std::cout << " (GBK/GB2312 - Chinese Simplified)";
        }
        std::cout << std::endl;
#endif
    }

    static void displayLocaleInfo() {
#ifndef _WIN32
        std::cout << "4. Unix Locale Information:" << std::endl;
        const char* locale = setlocale(LC_ALL, nullptr);
        std::cout << "   Current locale: " << (locale ? locale : "unknown") << std::endl;

        const char* lang = getenv("LANG");
        std::cout << "   LANG environment: " << (lang ? lang : "not set") << std::endl;
#endif
    }

    static void runCharacterTests() {
        std::cout << "5. Character Display Tests:" << std::endl;

        std::vector<TestCase> testCases = {
                {"ASCII", "Hello World!", "Basic ASCII characters"},
                {"Chinese Simplified", "你好世界！欢迎使用 C++ 项目脚手架",
                 "Simplified Chinese characters"},
                {"Chinese Traditional", "你好世界！歡迎使用 C++ 項目腳手架",
                 "Traditional Chinese characters"},
                {"Japanese", "こんにちは世界！C++プロジェクトへようこそ",
                 "Japanese hiragana and kanji"},
                {"Korean", "안녕하세요 세계! C++ 프로젝트에 오신 것을 환영합니다", "Korean Hangul"},
                {"Emojis", "🚀 🌟 💻 🔧 ⚡ 🎯 📦 🛠️", "Various emojis"},
                {"Symbols", "→ ← ↑ ↓ ✓ ✗ ★ ♦ ♠ ♣ ♥", "Special symbols"},
                {"Math", "∑ ∏ ∫ √ ∞ ≤ ≥ ≠ ± ÷ × π", "Mathematical symbols"},
                {"Box Drawing", "┌─┬─┐ │ │ │ ├─┼─┤ │ │ │ └─┴─┘", "Box drawing characters"},
                {"Accented", "Café naïve résumé Zürich", "Accented Latin characters"}};

        for (const auto& test : testCases) {
            std::cout << "   " << std::setw(20) << std::left << test.name << ": " << test.text
                      << std::endl;
            std::cout << "   " << std::setw(20) << " " << "  (" << test.description << ")"
                      << std::endl;
        }
    }

    static void testColoredUnicode() {
        std::cout << "6. Colored Unicode Output Test:" << std::endl;

        if (TerminalUtils::supportsAnsi()) {
            std::cout << "   " << TerminalUtils::colorize("红色中文 (Red Chinese)", Color::Red)
                      << std::endl;
            std::cout << "   " << TerminalUtils::colorize("绿色中文 (Green Chinese)", Color::Green)
                      << std::endl;
            std::cout << "   " << TerminalUtils::colorize("蓝色中文 (Blue Chinese)", Color::Blue)
                      << std::endl;
            std::cout << "   "
                      << TerminalUtils::colorize("🌈 彩色表情符号 (Colorful Emoji)", Color::Magenta)
                      << std::endl;

            // Test with background colors
            std::cout << "   "
                      << TerminalUtils::colorize("背景色测试 (Background Test)", Color::White,
                                                 Color::Blue)
                      << std::endl;
        } else {
            std::cout << "   ANSI colors not supported, showing plain text:" << std::endl;
            std::cout << "   红色中文 (Red Chinese)" << std::endl;
            std::cout << "   绿色中文 (Green Chinese)" << std::endl;
            std::cout << "   蓝色中文 (Blue Chinese)" << std::endl;
            std::cout << "   🌈 彩色表情符号 (Colorful Emoji)" << std::endl;
        }
    }
};

int main() {
    std::cout << "Starting UTF-8 encoding test..." << std::endl;

    try {
        EncodingTester::runAllTests();
    } catch (const std::exception& e) {
        std::cerr << "Error during encoding test: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error during encoding test" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();

    return 0;
}
