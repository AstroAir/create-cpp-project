#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    std::cout << "=== Simple UTF-8 Test ===" << std::endl;
    
#ifdef _WIN32
    // Display current code page
    UINT outputCP = GetConsoleOutputCP();
    UINT inputCP = GetConsoleCP();
    std::cout << "Current Output Code Page: " << outputCP << std::endl;
    std::cout << "Current Input Code Page: " << inputCP << std::endl;
    
    // Set UTF-8 code pages
    std::cout << "Setting UTF-8 code pages..." << std::endl;
    bool outputSet = SetConsoleOutputCP(CP_UTF8);
    bool inputSet = SetConsoleCP(CP_UTF8);
    
    std::cout << "Output CP set to UTF-8: " << (outputSet ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Input CP set to UTF-8: " << (inputSet ? "SUCCESS" : "FAILED") << std::endl;
    
    // Check new code pages
    outputCP = GetConsoleOutputCP();
    inputCP = GetConsoleCP();
    std::cout << "New Output Code Page: " << outputCP << std::endl;
    std::cout << "New Input Code Page: " << inputCP << std::endl;
#endif

    std::cout << std::endl;
    std::cout << "Testing character display:" << std::endl;
    std::cout << "ASCII: Hello World!" << std::endl;
    std::cout << "Chinese: 你好世界！" << std::endl;
    std::cout << "Japanese: こんにちは" << std::endl;
    std::cout << "Emoji: 🚀 🌟 💻" << std::endl;
    std::cout << "Symbols: ← → ↑ ↓ ✓ ✗" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Test complete. Press Enter to exit...";
    std::cin.get();
    
    return 0;
}
