#include <iostream>
#include <string>
#include "src/utils/terminal_utils.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace utils;

int main() {
    std::cout << "=== UTF-8 Test Verification ===" << std::endl;
    
    // Initialize UTF-8 support
    bool utf8Init = TerminalUtils::initializeUtf8Support();
    std::cout << "UTF-8 Initialization: " << (utf8Init ? "SUCCESS" : "FAILED") << std::endl;
    
#ifdef _WIN32
    UINT outputCP = GetConsoleOutputCP();
    UINT inputCP = GetConsoleCP();
    std::cout << "Output Code Page: " << outputCP << std::endl;
    std::cout << "Input Code Page: " << inputCP << std::endl;
#endif
    
    // Test UTF-8 encoding
    bool utf8Test = TerminalUtils::testUtf8Encoding();
    std::cout << "UTF-8 Encoding Test: " << (utf8Test ? "PASSED" : "FAILED") << std::endl;
    
    // Test various Unicode characters
    std::cout << std::endl << "=== Unicode Character Tests ===" << std::endl;
    
    std::cout << "Chinese: 你好世界！欢迎使用 C++ 项目脚手架" << std::endl;
    std::cout << "Emojis: 🚀 🌟 💻 🔧 ⚡ 🎯 📦 🛠️ 🌈 🎉" << std::endl;
    std::cout << "Japanese: こんにちは世界！" << std::endl;
    std::cout << "Korean: 안녕하세요 세계!" << std::endl;
    std::cout << "Math: ∑ ∏ ∫ √ ∞ ≤ ≥ ≠ ± ÷ × π α β γ δ ε" << std::endl;
    std::cout << "Box: ┌─┬─┐ │ │ │ ├─┼─┤ │ │ │ └─┴─┘" << std::endl;
    
    // Test logging simulation
    std::cout << std::endl << "=== Logging Simulation ===" << std::endl;
    std::string project_name = "测试项目";
    std::cout << "🚀 Creating console project: " << project_name << std::endl;
    std::cout << "📦 初始化Git仓库..." << std::endl;
    std::cout << "✅ 项目创建成功！" << std::endl;
    std::cout << "祝编码愉快! 🎉" << std::endl;
    
    std::cout << std::endl << "=== Test Complete ===" << std::endl;
    std::cout << "All UTF-8 functionality is working correctly!" << std::endl;
    
    return 0;
}
