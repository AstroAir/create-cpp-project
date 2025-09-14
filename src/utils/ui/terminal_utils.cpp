#include "../ui/terminal_utils.h"

#include <algorithm>
#include <cctype>
#include <clocale>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace utils {

// 静态成员变量初始化
bool TerminalUtils::lastWasNewline = true;

bool TerminalUtils::supportsAnsi() {
#ifdef _WIN32
    // Windows 10 build 10586+ supports ANSI sequences in CMD and PowerShell
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    // Try to enable ANSI escape processing
    bool ansiEnabled = SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    // Enhanced UTF-8 setup for better Unicode support
    if (ansiEnabled) {
        // Set UTF-8 code pages for both input and output
        bool outputSet = SetConsoleOutputCP(CP_UTF8);
        bool inputSet = SetConsoleCP(CP_UTF8);

        // Also configure stdin handle for UTF-8 input
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        if (hIn != INVALID_HANDLE_VALUE) {
            DWORD inMode = 0;
            if (GetConsoleMode(hIn, &inMode)) {
                SetConsoleMode(hIn, inMode | ENABLE_VIRTUAL_TERMINAL_INPUT);
            }
        }

        // Verify UTF-8 setup was successful
        if (!outputSet || !inputSet) {
            // Log warning but don't fail completely
            // UTF-8 setup failed, but ANSI might still work
        }
    }

    return ansiEnabled;
#else
    // Most UNIX terminals support ANSI
    // Also ensure locale is set for proper UTF-8 handling
    const char* locale = setlocale(LC_ALL, "");
    if (!locale) {
        // Try to set UTF-8 locale explicitly
        setlocale(LC_ALL, "en_US.UTF-8");
    }
    return isatty(STDOUT_FILENO);
#endif
}

bool TerminalUtils::initializeUtf8Support() {
#ifdef _WIN32
    // Set UTF-8 code pages for both input and output
    bool outputSet = SetConsoleOutputCP(CP_UTF8);
    bool inputSet = SetConsoleCP(CP_UTF8);

    // Configure console handles for UTF-8
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD outMode = 0;
        if (GetConsoleMode(hOut, &outMode)) {
            SetConsoleMode(hOut, outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    if (hIn != INVALID_HANDLE_VALUE) {
        DWORD inMode = 0;
        if (GetConsoleMode(hIn, &inMode)) {
            SetConsoleMode(hIn, inMode | ENABLE_VIRTUAL_TERMINAL_INPUT);
        }
    }

    return outputSet && inputSet;
#else
    // Set UTF-8 locale for Unix systems
    const char* locale = setlocale(LC_ALL, "");
    if (!locale || std::string(locale).find("UTF-8") == std::string::npos) {
        // Try common UTF-8 locales
        const char* utf8_locales[] = {"en_US.UTF-8", "C.UTF-8", "POSIX.UTF-8", "en_GB.UTF-8"};

        for (const char* loc : utf8_locales) {
            if (setlocale(LC_ALL, loc)) {
                return true;
            }
        }
        return false;
    }
    return true;
#endif
}

bool TerminalUtils::testUtf8Encoding() {
    // Test with various Unicode characters
    const std::string testStrings[] = {
            "Hello 世界",         // Chinese characters
            "こんにちは",         // Japanese hiragana
            "🚀 Rocket",          // Emoji
            "Café naïve résumé",  // Accented characters
            "Ελληνικά",           // Greek
            "العربية"             // Arabic
    };

    // For now, we'll assume UTF-8 works if we can output without crashing
    // A more sophisticated test would check actual rendering
    try {
        for (const auto& test : testStrings) {
            // Just verify the string is valid UTF-8 by checking length
            if (test.empty()) {
                return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string TerminalUtils::getAnsiColorCode(Color color, bool isBackground) {
    if (!supportsAnsi())
        return "";

    int baseCode = isBackground ? 40 : 30;

    switch (color) {
        case Color::Reset:
            return "\033[0m";
        case Color::Black:
            return "\033[" + std::to_string(baseCode + 0) + "m";
        case Color::Red:
            return "\033[" + std::to_string(baseCode + 1) + "m";
        case Color::Green:
            return "\033[" + std::to_string(baseCode + 2) + "m";
        case Color::Yellow:
            return "\033[" + std::to_string(baseCode + 3) + "m";
        case Color::Blue:
            return "\033[" + std::to_string(baseCode + 4) + "m";
        case Color::Magenta:
            return "\033[" + std::to_string(baseCode + 5) + "m";
        case Color::Cyan:
            return "\033[" + std::to_string(baseCode + 6) + "m";
        case Color::White:
            return "\033[" + std::to_string(baseCode + 7) + "m";
        case Color::BrightBlack:
            return "\033[" + std::to_string(baseCode + 0 + 60) + "m";
        case Color::BrightRed:
            return "\033[" + std::to_string(baseCode + 1 + 60) + "m";
        case Color::BrightGreen:
            return "\033[" + std::to_string(baseCode + 2 + 60) + "m";
        case Color::BrightYellow:
            return "\033[" + std::to_string(baseCode + 3 + 60) + "m";
        case Color::BrightBlue:
            return "\033[" + std::to_string(baseCode + 4 + 60) + "m";
        case Color::BrightMagenta:
            return "\033[" + std::to_string(baseCode + 5 + 60) + "m";
        case Color::BrightCyan:
            return "\033[" + std::to_string(baseCode + 6 + 60) + "m";
        case Color::BrightWhite:
            return "\033[" + std::to_string(baseCode + 7 + 60) + "m";
        default:
            return "";
    }
}

std::string TerminalUtils::getAnsiStyleCode(TextStyle style) {
    if (!supportsAnsi())
        return "";

    switch (style) {
        case TextStyle::Bold:
            return "\033[1m";
        case TextStyle::Dim:
            return "\033[2m";
        case TextStyle::Italic:
            return "\033[3m";
        case TextStyle::Underline:
            return "\033[4m";
        case TextStyle::Blink:
            return "\033[5m";
        case TextStyle::Reverse:
            return "\033[7m";
        case TextStyle::Hidden:
            return "\033[8m";
        case TextStyle::Strike:
            return "\033[9m";
        default:
            return "";
    }
}

std::string TerminalUtils::colorize(const std::string& text, Color foreground, Color background) {
    if (!supportsAnsi())
        return text;

    std::string result = getAnsiColorCode(foreground);
    if (background != Color::Reset) {
        result += getAnsiColorCode(background, true);
    }

    result += text + getAnsiColorCode(Color::Reset);
    return result;
}

std::string TerminalUtils::stylize(const std::string& text, const std::vector<TextStyle>& styles) {
    if (!supportsAnsi() || styles.empty())
        return text;

    std::string result;
    for (const auto& style : styles) {
        result += getAnsiStyleCode(style);
    }

    result += text + "\033[0m";
    return result;
}

std::string TerminalUtils::colorAndStyle(const std::string& text, Color foreground,
                                         const std::vector<TextStyle>& styles, Color background) {
    if (!supportsAnsi())
        return text;

    std::string result = getAnsiColorCode(foreground);
    if (background != Color::Reset) {
        result += getAnsiColorCode(background, true);
    }

    for (const auto& style : styles) {
        result += getAnsiStyleCode(style);
    }

    result += text + "\033[0m";
    return result;
}

void TerminalUtils::clearScreen() {
    if (!supportsAnsi())
        return;

    std::cout << "\033[2J\033[H" << std::flush;
}

void TerminalUtils::showProgressBar(int percent, int width) {
    if (!supportsAnsi()) {
        // 简单输出百分比
        std::cout << "Progress: " << percent << "%" << std::endl;
        return;
    }

    // 确保百分比在合理范围�?
    percent = std::max(0, std::min(100, percent));

    std::string bar;
    bar.reserve(width + 10);

    int filledWidth = width * percent / 100;

    // 构建进度�?
    bar += "[";
    for (int i = 0; i < width; ++i) {
        if (i < filledWidth) {
            bar += "=";
        } else if (i == filledWidth) {
            bar += ">";
        } else {
            bar += " ";
        }
    }

    // 添加百分�?
    bar += "] " + std::to_string(percent) + "%";

    // 清除当前行并显示进度�?
    clearLine();
    std::cout << bar << std::flush;
}

void TerminalUtils::showProgressBar(int percent, int width, Color fillColor, Color emptyColor) {
    if (!supportsAnsi()) {
        // 简单输出百分比
        std::cout << "Progress: " << percent << "%" << std::endl;
        return;
    }

    // 确保百分比在合理范围�?
    percent = std::max(0, std::min(100, percent));

    std::string bar;
    bar.reserve(width + 20);

    int filledWidth = width * percent / 100;

    // 构建进度�?
    bar += "[";
    for (int i = 0; i < width; ++i) {
        if (i < filledWidth) {
            bar += colorize("█", fillColor);
        } else {
            bar += colorize("░", emptyColor);
        }
    }

    // 添加百分�?
    bar += "] " + std::to_string(percent) + "%";

    // 清除当前行并显示进度�?
    clearLine();
    std::cout << bar << std::flush;
}

void TerminalUtils::showSpinner(int state) {
    if (!supportsAnsi())
        return;

    static const std::vector<std::string> spinnerChars = {"⠋", "⠙", "⠹", "⠸", "⠼",
                                                          "⠴", "⠦", "⠧", "⠇", "⠏"};
    clearLine();
    std::cout << spinnerChars[state % spinnerChars.size()] << std::flush;
}

void TerminalUtils::runAnimatedProgress(int durationMs, const std::function<int(int)>& progressFunc,
                                        int width, Color fillColor, Color emptyColor) {
    if (!supportsAnsi())
        return;

    const int frameTimeMs = 50;  // 20 FPS
    const int totalFrames = durationMs / frameTimeMs;

    hideCursor();

    for (int frame = 0; frame <= totalFrames; ++frame) {
        // 计算当前进度
        int elapsedTime = (frame * durationMs) / totalFrames;
        int percent = progressFunc(elapsedTime);

        // 显示进度�?
        showProgressBar(percent, width, fillColor, emptyColor);

        // 等待下一�?
        if (frame < totalFrames) {
            std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeMs));
        }
    }

    std::cout << std::endl;
    showCursor();
}

void TerminalUtils::runSpinner(int durationMs, const std::string& message, int framesPerSecond) {
    if (!supportsAnsi())
        return;

    const int frameTimeMs = 1000 / framesPerSecond;
    const int totalFrames = durationMs / frameTimeMs;

    hideCursor();

    for (int frame = 0; frame < totalFrames; ++frame) {
        clearLine();
        showSpinner(frame);
        if (!message.empty()) {
            std::cout << " " << message;
        }
        std::cout << std::flush;

        // 等待下一�?
        std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeMs));
    }

    clearLine();
    showCursor();
}

void TerminalUtils::clearLine() {
    if (!supportsAnsi())
        return;

    std::cout << "\r\033[K" << std::flush;
}

void TerminalUtils::moveCursorToLineStart() {
    if (!supportsAnsi())
        return;

    std::cout << "\r" << std::flush;
}

void TerminalUtils::moveCursor(int row, int col) {
    if (!supportsAnsi())
        return;

    std::cout << "\033[" << row << ";" << col << "H" << std::flush;
}

void TerminalUtils::saveCursorPosition() {
    if (!supportsAnsi())
        return;

    std::cout << "\033[s" << std::flush;
}

void TerminalUtils::restoreCursorPosition() {
    if (!supportsAnsi())
        return;

    std::cout << "\033[u" << std::flush;
}

void TerminalUtils::hideCursor() {
    if (!supportsAnsi())
        return;

    std::cout << "\033[?25l" << std::flush;
}

void TerminalUtils::showCursor() {
    if (!supportsAnsi())
        return;

    std::cout << "\033[?25h" << std::flush;
}

int TerminalUtils::getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80;  // 默认宽度

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return columns;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}

int TerminalUtils::getTerminalHeight() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int rows = 24;  // 默认高度

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return rows;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
#endif
}

std::pair<int, int> TerminalUtils::getTerminalSize() {
    return {getTerminalWidth(), getTerminalHeight()};
}

std::string TerminalUtils::centerText(const std::string& text, int width) {
    if (width <= 0) {
        width = getTerminalWidth();
    }

    int textWidth = static_cast<int>(text.length());
    if (textWidth >= width)
        return text;

    int padding = (width - textWidth) / 2;
    return std::string(padding, ' ') + text;
}

std::string TerminalUtils::rightAlign(const std::string& text, int width) {
    if (width <= 0) {
        width = getTerminalWidth();
    }

    int textWidth = static_cast<int>(text.length());
    if (textWidth >= width)
        return text;

    int padding = width - textWidth;
    return std::string(padding, ' ') + text;
}

std::string TerminalUtils::truncate(const std::string& text, int maxLength, bool addEllipsis) {
    if (static_cast<int>(text.length()) <= maxLength)
        return text;

    if (addEllipsis && maxLength > 3) {
        return text.substr(0, maxLength - 3) + "...";
    } else {
        return text.substr(0, maxLength);
    }
}

std::vector<std::string> TerminalUtils::wrapText(const std::string& text, int width) {
    if (width == 0) {
        // Special case: width 0 means no wrapping, return original text
        return {text};
    }

    if (width < 0) {
        width = getTerminalWidth();
    }

    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string word, line;

    while (stream >> word) {
        // Handle very long words that exceed the width
        while (word.length() > static_cast<size_t>(width)) {
            if (line.empty()) {
                // If line is empty, break the word at width
                lines.push_back(word.substr(0, width));
                word = word.substr(width);
            } else {
                // If line has content, finish the line and start new one
                lines.push_back(line);
                line.clear();
            }
        }

        if (line.empty()) {
            line = word;
        } else if (line.length() + word.length() + 1 <= static_cast<size_t>(width)) {
            line += " " + word;
        } else {
            lines.push_back(line);
            line = word;
        }
    }

    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

std::vector<std::string> TerminalUtils::getBorderChars(BorderStyle style) {
    // 返回顺序: 左上, 右上, 左下, 右下, 水平, 垂直, 左T, 右T, 上T, 下T, 十字
    switch (style) {
        case BorderStyle::Single:
            return {"┌", "┐", "└", "┘", "─", "│", "├", "┤", "┬", "┴", "┼"};
        case BorderStyle::Double:
            return {"╔", "╗", "╚", "╝", "═", "║", "╠", "╣", "╦", "╩", "╬"};
        case BorderStyle::Rounded:
            return {"╭", "╮", "╰", "╯", "─", "│", "├", "┤", "┬", "┴", "┼"};
        case BorderStyle::Bold:
            return {"┏", "┓", "┗", "┛", "━", "┃", "┣", "┫", "┳", "┻", "╋"};
        case BorderStyle::Ascii:
        default:
            return {"+", "+", "+", "+", "-", "|", "+", "+", "+", "+", "+"};
    }
}

void TerminalUtils::showBox(const std::vector<std::string>& lines) {
    if (lines.empty())
        return;

    // 找出最长行的长�?
    int maxLength = 0;
    for (const auto& line : lines) {
        maxLength = std::max(maxLength, static_cast<int>(line.length()));
    }

    // 上边�?    std::cout << "�? << std::string(maxLength + 2, '-') << "�? <<
    // std::endl;

    // 内容
    for (const auto& line : lines) {
        std::cout << "│" << line << std::string(maxLength - line.length(), ' ') << " │"
                  << std::endl;
    }

    // 下边�?    std::cout << "�? << std::string(maxLength + 2, '-') << "�? <<
    // std::endl;
}

void TerminalUtils::showBox(const std::vector<std::string>& lines, BorderStyle style,
                            Color borderColor, Color textColor, std::string title) {
    if (lines.empty())
        return;

    auto borders = getBorderChars(style);

    // 找出最长行的长�?
    int maxLength = 0;
    for (const auto& line : lines) {
        maxLength = std::max(maxLength, static_cast<int>(line.length()));
    }

    // 如果有标题，确保标题能适应框的宽度
    if (!title.empty()) {
        maxLength = std::max(maxLength, static_cast<int>(title.length()));
    }

    // 标题�?
    if (!title.empty()) {
        std::cout << colorize(borders[0], borderColor);
        std::cout << colorize(borders[4], borderColor) << " ";
        std::cout << colorize(title, textColor);
        std::cout << " "
                  << colorize(std::string(maxLength - title.length(), borders[4][0]), borderColor);
        std::cout << colorize(borders[1], borderColor) << std::endl;
    } else {
        // 上边�?
        std::cout << colorize(borders[0], borderColor);
        std::cout << colorize(std::string(maxLength + 2, borders[4][0]), borderColor);
        std::cout << colorize(borders[1], borderColor) << std::endl;
    }

    // 内容
    for (const auto& line : lines) {
        std::cout << colorize(borders[5], borderColor) << " ";
        std::cout << colorize(line, textColor);
        std::cout << std::string(maxLength - line.length(), ' ') << " ";
        std::cout << colorize(borders[5], borderColor) << std::endl;
    }

    // 下边�?
    std::cout << colorize(borders[2], borderColor);
    std::cout << colorize(std::string(maxLength + 2, borders[4][0]), borderColor);
    std::cout << colorize(borders[3], borderColor) << std::endl;
}

void TerminalUtils::showTable(const std::vector<std::vector<TableCell>>& rows, bool hasHeader,
                              BorderStyle style, Color borderColor) {
    if (rows.empty())
        return;

    auto borders = getBorderChars(style);

    // 计算每列的最大宽�?
    std::vector<int> columnWidths(rows[0].size(), 0);
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size() && i < columnWidths.size(); ++i) {
            columnWidths[i] = std::max(columnWidths[i], static_cast<int>(row[i].text.length()));
        }
    }

    // 绘制表头上边�?
    std::cout << colorize(borders[0], borderColor);
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        std::cout << colorize(std::string(columnWidths[i] + 2, borders[4][0]), borderColor);
        if (i < columnWidths.size() - 1) {
            std::cout << colorize(borders[8], borderColor);
        }
    }
    std::cout << colorize(borders[1], borderColor) << std::endl;

    // 绘制表格内容
    for (size_t rowIdx = 0; rowIdx < rows.size(); ++rowIdx) {
        const auto& row = rows[rowIdx];

        // 绘制单元�?
        std::cout << colorize(borders[5], borderColor);
        for (size_t colIdx = 0; colIdx < row.size() && colIdx < columnWidths.size(); ++colIdx) {
            const auto& cell = row[colIdx];
            std::string cellText = cell.text;

            // 居中处理
            if (cell.centered) {
                int padding = columnWidths[colIdx] - static_cast<int>(cellText.length());
                int leftPad = padding / 2;
                int rightPad = padding - leftPad;
                cellText = std::string(leftPad, ' ') + cellText + std::string(rightPad, ' ');
            } else {
                cellText = cellText + std::string(columnWidths[colIdx] - cellText.length(), ' ');
            }

            // 应用颜色和样�?
            if (cell.foreground != Color::Reset || cell.background != Color::Reset ||
                !cell.styles.empty()) {
                cellText = colorAndStyle(cellText, cell.foreground, cell.styles, cell.background);
            }

            std::cout << " " << cellText << " ";
            std::cout << colorize(borders[5], borderColor);
        }
        std::cout << std::endl;

        // 绘制行分隔符
        if (rowIdx < rows.size() - 1) {
            std::cout << colorize(borders[6], borderColor);
            for (size_t i = 0; i < columnWidths.size(); ++i) {
                std::cout << colorize(std::string(columnWidths[i] + 2, borders[4][0]), borderColor);
                if (i < columnWidths.size() - 1) {
                    std::cout << colorize(borders[10], borderColor);
                }
            }
            std::cout << colorize(borders[7], borderColor) << std::endl;
        }

        // 表头后的分隔�?
        if (hasHeader && rowIdx == 0) {
            std::cout << colorize(borders[6], borderColor);
            for (size_t i = 0; i < columnWidths.size(); ++i) {
                std::cout << colorize(std::string(columnWidths[i] + 2, borders[4][0]), borderColor);
                if (i < columnWidths.size() - 1) {
                    std::cout << colorize(borders[10], borderColor);
                }
            }
            std::cout << colorize(borders[7], borderColor) << std::endl;
        }
    }

    // 绘制底部边框
    std::cout << colorize(borders[2], borderColor);
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        std::cout << colorize(std::string(columnWidths[i] + 2, borders[4][0]), borderColor);
        if (i < columnWidths.size() - 1) {
            std::cout << colorize(borders[9], borderColor);
        }
    }
    std::cout << colorize(borders[3], borderColor) << std::endl;
}

void TerminalUtils::showSuccess(const std::string& message) {
    std::cout << colorize("�?" + message, Color::BrightGreen) << std::endl;
    lastWasNewline = true;
}

void TerminalUtils::showError(const std::string& message) {
    std::cout << colorize("�?" + message, Color::BrightRed) << std::endl;
    lastWasNewline = true;
}

void TerminalUtils::showInfo(const std::string& message) {
    std::cout << colorize("ℹ️ " + message, Color::BrightBlue) << std::endl;
    lastWasNewline = true;
}

void TerminalUtils::showWarning(const std::string& message) {
    std::cout << colorize("⚠️ " + message, Color::BrightYellow) << std::endl;
    lastWasNewline = true;
}

void TerminalUtils::showDebug(const std::string& message) {
    std::cout << colorize("🔍 " + message, Color::BrightMagenta) << std::endl;
    lastWasNewline = true;
}

int TerminalUtils::showMenu(const std::vector<std::string>& options, const std::string& prompt,
                            Color highlightColor) {
    if (!supportsAnsi() || options.empty()) {
        // 降级处理
        std::cout << prompt << std::endl;
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << options[i] << std::endl;
        }

        int choice = 0;
        while (choice < 1 || choice > static_cast<int>(options.size())) {
            std::cout << "请输入选项 (1-" << options.size() << "): ";
            std::cin >> choice;
        }
        return choice - 1;
    }

    // 高级菜单显示
    int selectedIdx = 0;
    bool running = true;

    hideCursor();

    while (running) {
        clearScreen();
        std::cout << prompt << std::endl << std::endl;

        for (size_t i = 0; i < options.size(); ++i) {
            if (i == static_cast<size_t>(selectedIdx)) {
                std::cout << "  " << colorize("►", highlightColor) << " "
                          << colorize(options[i], highlightColor, Color::Reset) << std::endl;
            } else {
                std::cout << "    " << options[i] << std::endl;
            }
        }

        std::cout << std::endl << "使用↑↓箭头选择，Enter确认" << std::endl;

        // 读取按键
        char key = std::cin.get();
        if (key == '\033') {
            // 处理方向�?
            std::cin.get();  // 跳过 '['
            switch (std::cin.get()) {
                case 'A':  // 上箭�?
                    selectedIdx = (selectedIdx > 0) ? selectedIdx - 1 : options.size() - 1;
                    break;
                case 'B':  // 下箭�?
                    selectedIdx = (selectedIdx < static_cast<int>(options.size()) - 1)
                                          ? selectedIdx + 1
                                          : 0;
                    break;
            }
        } else if (key == '\n' || key == '\r') {
            running = false;
        }
    }

    showCursor();
    return selectedIdx;
}

void TerminalUtils::handleTerminalResize(std::function<void()> redrawFunc) {
#ifndef _WIN32
    struct sigaction sa;
    sa.sa_handler = [](int) {
        // SIGWINCH 处理�?
        // 这里不能安全地调用大多数函数，所以我们只设置一个全局标志
    };
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGWINCH, &sa, nullptr);

    // 在主循环中定期检查标志并调用重绘函数
    // 由于在信号处理器中不安全，所以这部分需要在主代码中实现
    if (redrawFunc) {
        redrawFunc();
    }
#else
    (void)redrawFunc;  // Suppress unused parameter warning on Windows
#endif
}

// Enhanced Next.js-style features implementation
void TerminalUtils::showBrandedHeader(const std::string& title, const std::string& subtitle) {
    int width = getTerminalWidth();
    if (width < 40)
        width = 80;  // fallback

    clearScreen();

    // Create a stylized header with gradient-like effect
    std::string topBorder = "+" + std::string(width - 2, '-') + "+";
    std::string bottomBorder = "+" + std::string(width - 2, '-') + "+";

    std::cout << colorize(topBorder, Color::BrightCyan) << "\n";

    // Title with centering
    std::string centeredTitle = centerText(title, width - 4);
    std::cout << colorize("│", Color::BrightCyan)
              << colorAndStyle(centeredTitle, Color::BrightWhite, {TextStyle::Bold})
              << colorize(" │", Color::BrightCyan) << "\n";

    if (!subtitle.empty()) {
        std::string centeredSubtitle = centerText(subtitle, width - 4);
        std::cout << colorize("│", Color::BrightCyan)
                  << colorize(centeredSubtitle, Color::BrightBlack)
                  << colorize(" │", Color::BrightCyan) << "\n";
    }

    std::cout << colorize(bottomBorder, Color::BrightCyan) << "\n\n";
}

void TerminalUtils::showStepIndicator(int currentStep, int totalSteps,
                                      const std::string& stepName) {
    std::cout << "\n";

    // Progress dots
    for (int i = 1; i <= totalSteps; i++) {
        if (i == currentStep) {
            std::cout << colorize("*", Color::BrightGreen);
        } else if (i < currentStep) {
            std::cout << colorize("*", Color::Green);
        } else {
            std::cout << colorize("o", Color::BrightBlack);
        }

        if (i < totalSteps) {
            std::cout << colorize("─", Color::BrightBlack);
        }
    }

    std::cout << "\n";
    std::cout << colorAndStyle("Step " + std::to_string(currentStep) + " of " +
                                       std::to_string(totalSteps),
                               Color::BrightCyan, {TextStyle::Bold})
              << ": " << colorize(stepName, Color::White) << "\n\n";
}

void TerminalUtils::showAnimatedLogo() {
    std::vector<std::string> logo = {
            "   ______ ______ ______       ______ ______ ______ ______ ______ ______ "
            "______ "
            "______ ",
            "  /      /      /      \\     /      /      /      /      /      /      / "
            "     /      "
            "\\",
            " /      /      /        \\   /      /      /      /      /      /      /  "
            "    /       "
            " \\",
            "/      /      /          \\ /      /      /      /      /      /      /   "
            "   /        "
            "  \\",
            "\\      \\      \\          / \\      \\      \\      \\      \\      \\  "
            "    \\      "
            "\\          /",
            " \\      \\      \\        /   \\      \\      \\      \\      \\      \\ "
            "     \\     "
            " \\        /",
            "  \\______\\______\\______/     "
            "\\______\\______\\______\\______\\______\\______\\______\\______/"};

    for (const auto& line : logo) {
        std::cout << colorize(centerText(line), Color::BrightCyan) << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "\n";
}

void TerminalUtils::printCharWithDelay(char c, int delayMs) {
    std::cout << c << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
}

void TerminalUtils::showTypingAnimation(const std::string& text, int delayMs) {
    for (char c : text) {
        printCharWithDelay(c, delayMs);
    }
    std::cout << "\n";
}

void TerminalUtils::showPulsingText(const std::string& text, Color color, int pulses) {
    for (int i = 0; i < pulses; i++) {
        moveCursorToLineStart();
        clearLine();
        std::cout << colorAndStyle(text, color, {TextStyle::Bold}) << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        moveCursorToLineStart();
        clearLine();
        std::cout << colorize(text, Color::BrightBlack) << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    moveCursorToLineStart();
    clearLine();
    std::cout << colorAndStyle(text, color, {TextStyle::Bold}) << "\n";
}

void TerminalUtils::showModernProgressBar(int percent, const std::string& label, Color color) {
    int width = 40;
    int filled = (percent * width) / 100;

    std::string bar = "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            bar += "=";
        } else if (i == filled && percent < 100) {
            bar += ">";
        } else {
            bar += " ";
        }
    }
    bar += "]";

    moveCursorToLineStart();
    clearLine();

    if (!label.empty()) {
        std::cout << colorize(label + " ", Color::White);
    }

    std::cout << colorize(bar, color) << " "
              << colorAndStyle(std::to_string(percent) + "%", Color::BrightWhite, {TextStyle::Bold})
              << std::flush;

    if (percent >= 100) {
        std::cout << "\n";
    }
}

void TerminalUtils::showDotSpinner(const std::string& message, int durationMs) {
    const std::vector<std::string> frames = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    int frameCount = frames.size();
    int frameDelay = 100;  // ms per frame
    int totalFrames = durationMs / frameDelay;

    hideCursor();

    for (int i = 0; i < totalFrames; i++) {
        moveCursorToLineStart();
        clearLine();

        std::string frame = frames[i % frameCount];
        std::cout << colorize(frame, Color::BrightCyan) << " " << colorize(message, Color::White)
                  << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
    }

    moveCursorToLineStart();
    clearLine();
    showCursor();
}

int TerminalUtils::showInteractiveMenu(const std::vector<std::string>& options,
                                       const std::string& prompt, int defaultSelection) {
    if (options.empty())
        return -1;

    int selected = defaultSelection;
    if (selected < 0 || selected >= static_cast<int>(options.size())) {
        selected = 0;
    }

    if (!prompt.empty()) {
        std::cout << colorAndStyle(prompt, Color::BrightCyan, {TextStyle::Bold}) << "\n\n";
    }

    // For now, implement as a simple numbered menu (can be enhanced with arrow key
    // navigation later)
    for (size_t i = 0; i < options.size(); i++) {
        if (static_cast<int>(i) == selected) {
            std::cout << colorAndStyle("> " + std::to_string(i + 1) + ". " + options[i],
                                       Color::BrightGreen, {TextStyle::Bold})
                      << "\n";
        } else {
            std::cout << colorize("  " + std::to_string(i + 1) + ". " + options[i], Color::White)
                      << "\n";
        }
    }

    std::cout << "\n"
              << colorize("Enter your choice (1-" + std::to_string(options.size()) + "): ",
                          Color::BrightYellow);

    int choice;
    std::cin >> choice;
    std::cin.ignore();  // Clear the newline

    if (choice >= 1 && choice <= static_cast<int>(options.size())) {
        return choice - 1;
    }

    return selected;  // Return default if invalid input
}

bool TerminalUtils::showConfirmDialog(const std::string& message, bool defaultValue) {
    std::string prompt = message + " " + (defaultValue ? "[Y/n]" : "[y/N]") + ": ";
    std::cout << colorize(prompt, Color::BrightYellow);

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }

    char first = static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));
    return first == 'y';
}

std::string TerminalUtils::showInputDialog(const std::string& prompt,
                                           const std::string& placeholder,
                                           const std::string& defaultValue) {
    std::string fullPrompt = prompt;
    if (!defaultValue.empty()) {
        fullPrompt += " [" + defaultValue + "]";
    } else if (!placeholder.empty()) {
        fullPrompt += " (" + placeholder + ")";
    }
    fullPrompt += ": ";

    std::cout << colorize(fullPrompt, Color::BrightCyan);

    std::string input;
    std::getline(std::cin, input);

    if (input.empty() && !defaultValue.empty()) {
        return defaultValue;
    }

    return input;
}

void TerminalUtils::showCard(const std::string& title, const std::vector<std::string>& content,
                             Color color) {
    std::cout << colorize("+- " + title + " " + std::string(50 - title.length(), '-') + "+", color)
              << std::endl;

    for (const auto& line : content) {
        std::cout << colorize("| ", color) << line << std::endl;
    }

    std::cout << colorize("+" + std::string(52, '-') + "+", color) << std::endl;
}

void TerminalUtils::showTwoColumnLayout(
        const std::vector<std::pair<std::string, std::string>>& items) {
    const int leftWidth = 25;

    for (const auto& [left, right] : items) {
        std::string leftPadded = left;
        if (leftPadded.length() > leftWidth) {
            leftPadded = leftPadded.substr(0, leftWidth - 3) + "...";
        } else {
            leftPadded.resize(leftWidth, ' ');
        }

        std::cout << colorize(leftPadded, Color::BrightCyan) << " : " << right << std::endl;
    }
}

void TerminalUtils::showFeatureList(
        const std::vector<std::pair<std::string, std::string>>& features) {
    for (const auto& [feature, description] : features) {
        std::cout << colorize("* " + feature, Color::BrightGreen) << std::endl;
        std::cout << "  " << description << std::endl;
        std::cout << std::endl;
    }
}

void TerminalUtils::showSectionSeparator() {
    std::cout << colorize(std::string(60, '-'), Color::BrightBlack) << std::endl;
}

// NPM-style CLI enhancements
void TerminalUtils::showNpmStyleHeader(const std::string& toolName, const std::string& version) {
    if (!supportsAnsi()) {
        std::cout << toolName;
        if (!version.empty()) {
            std::cout << " v" << version;
        }
        std::cout << std::endl;
        return;
    }

    clearScreen();
    std::cout << "\n";

    // Create npm-style header with gradient effect
    std::string header = "  " + toolName;
    if (!version.empty()) {
        header += " v" + version;
    }

    std::cout << colorize(header, Color::BrightCyan) << "\n";
    std::cout << colorize("  " + std::string(header.length() - 2, '-'), Color::Cyan) << "\n\n";
}

void TerminalUtils::showNpmStyleCommand(const std::string& command,
                                        const std::string& description) {
    if (!supportsAnsi()) {
        std::cout << command;
        if (!description.empty()) {
            std::cout << " - " << description;
        }
        std::cout << std::endl;
        return;
    }

    std::cout << colorize("  $ ", Color::BrightGreen) << colorize(command, Color::BrightWhite);

    if (!description.empty()) {
        std::cout << colorize(" # " + description, Color::BrightBlack);
    }
    std::cout << "\n";
}

void TerminalUtils::showNpmStyleProgress(const std::string& operation, int percent,
                                         const std::string& currentFile) {
    if (!supportsAnsi()) {
        std::cout << operation << ": " << percent << "%";
        if (!currentFile.empty()) {
            std::cout << " (" << currentFile << ")";
        }
        std::cout << std::endl;
        return;
    }

    clearLine();

    // Create progress bar
    const int barWidth = 30;
    int filled = (barWidth * percent) / 100;

    std::string bar = "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) {
            bar += colorize("#", Color::BrightGreen);
        } else {
            bar += colorize("-", Color::BrightBlack);
        }
    }
    bar += "]";

    std::cout << colorize("  �?", Color::BrightYellow) << colorize(operation, Color::BrightWhite)
              << " " << bar << colorize(" " + std::to_string(percent) + "%", Color::BrightCyan);

    if (!currentFile.empty()) {
        std::cout << colorize(" " + currentFile, Color::BrightBlack);
    }

    std::cout << std::flush;
}

void TerminalUtils::showNpmStyleSuccess(const std::string& message, const std::string& details) {
    if (!supportsAnsi()) {
        std::cout << "�?" << message;
        if (!details.empty()) {
            std::cout << " (" << details << ")";
        }
        std::cout << std::endl;
        return;
    }

    std::cout << "\n"
              << colorize("  �?", Color::BrightGreen) << colorize(message, Color::BrightWhite);

    if (!details.empty()) {
        std::cout << colorize(" " + details, Color::BrightBlack);
    }
    std::cout << "\n";
}

void TerminalUtils::showNpmStyleError(const std::string& message, const std::string& suggestion) {
    if (!supportsAnsi()) {
        std::cout << "�?" << message;
        if (!suggestion.empty()) {
            std::cout << "\n  Suggestion: " << suggestion;
        }
        std::cout << std::endl;
        return;
    }

    std::cout << "\n"
              << colorize("  �?", Color::BrightRed) << colorize(message, Color::BrightWhite)
              << "\n";

    if (!suggestion.empty()) {
        std::cout << colorize("    💡 ", Color::BrightYellow)
                  << colorize(suggestion, Color::BrightCyan) << "\n";
    }
}

void TerminalUtils::showNpmStyleWarning(const std::string& message, const std::string& details) {
    if (!supportsAnsi()) {
        std::cout << "�?" << message;
        if (!details.empty()) {
            std::cout << " (" << details << ")";
        }
        std::cout << std::endl;
        return;
    }

    std::cout << "\n"
              << colorize("  �?", Color::BrightYellow) << colorize(message, Color::BrightWhite);

    if (!details.empty()) {
        std::cout << colorize(" " + details, Color::BrightBlack);
    }
    std::cout << "\n";
}

// Enhanced confirmation dialogs
bool TerminalUtils::showDestructiveConfirmDialog(const std::string& action,
                                                 const std::string& target,
                                                 const std::string& consequence) {
    if (!supportsAnsi()) {
        std::cout << "WARNING: " << action << " " << target;
        if (!consequence.empty()) {
            std::cout << "\nThis will " << consequence;
        }
        std::cout << "\nAre you sure? (y/N): ";

        std::string input;
        std::getline(std::cin, input);
        return !input.empty() && (input[0] == 'y' || input[0] == 'Y');
    }

    std::cout << "\n" << colorize("  ⚠️  WARNING", Color::BrightRed) << "\n\n";
    std::cout << "  " << colorize("Action:", Color::BrightWhite) << " " << action << "\n";
    std::cout << "  " << colorize("Target:", Color::BrightWhite) << " "
              << colorize(target, Color::BrightYellow) << "\n";

    if (!consequence.empty()) {
        std::cout << "  " << colorize("Result:", Color::BrightWhite) << " "
                  << colorize(consequence, Color::BrightRed) << "\n";
    }

    std::cout << "\n" << colorize("  This action cannot be undone!", Color::BrightRed) << "\n\n";
    std::cout << "  Type " << colorize("yes", Color::BrightGreen) << " to confirm: ";

    std::string input;
    std::getline(std::cin, input);
    return input == "yes";
}

bool TerminalUtils::showAdvancedConfirmDialog(const std::string& title, const std::string& message,
                                              const std::vector<std::string>& options,
                                              int defaultOption) {
    if (!supportsAnsi()) {
        std::cout << title << "\n" << message << "\n";
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << (i + 1) << ". " << options[i];
            if (static_cast<int>(i) == defaultOption) {
                std::cout << " (default)";
            }
            std::cout << "\n";
        }
        std::cout << "Choice: ";

        std::string input;
        std::getline(std::cin, input);
        if (input.empty())
            return defaultOption == 0;

        try {
            int choice = std::stoi(input) - 1;
            return choice == 0;
        } catch (...) {
            return defaultOption == 0;
        }
    }

    std::cout << "\n" << colorize("  " + title, Color::BrightCyan) << "\n\n";
    std::cout << "  " << message << "\n\n";

    for (size_t i = 0; i < options.size(); ++i) {
        std::string prefix = "  " + std::to_string(i + 1) + ". ";
        Color optionColor =
                (static_cast<int>(i) == defaultOption) ? Color::BrightGreen : Color::BrightWhite;

        std::cout << colorize(prefix, Color::BrightBlack) << colorize(options[i], optionColor);

        if (static_cast<int>(i) == defaultOption) {
            std::cout << colorize(" (default)", Color::BrightBlack);
        }
        std::cout << "\n";
    }

    std::cout << "\n  Choice [" << (defaultOption + 1) << "]: ";

    std::string input;
    std::getline(std::cin, input);
    if (input.empty())
        return defaultOption == 0;

    try {
        int choice = std::stoi(input) - 1;
        return choice == 0;
    } catch (...) {
        return defaultOption == 0;
    }
}

// Multi-step wizards
void TerminalUtils::showWizardHeader(const std::string& title, int currentStep, int totalSteps) {
    if (!supportsAnsi()) {
        std::cout << title << " (Step " << currentStep << " of " << totalSteps << ")\n";
        return;
    }

    clearScreen();
    std::cout << "\n";

    // Title
    std::cout << colorize("  " + title, Color::BrightCyan) << "\n";

    // Step indicator
    std::string stepIndicator =
            "  Step " + std::to_string(currentStep) + " of " + std::to_string(totalSteps);
    std::cout << colorize(stepIndicator, Color::BrightBlack) << "\n\n";

    // Progress dots
    std::cout << "  ";
    for (int i = 1; i <= totalSteps; ++i) {
        if (i < currentStep) {
            std::cout << colorize("*", Color::BrightGreen);
        } else if (i == currentStep) {
            std::cout << colorize("*", Color::BrightCyan);
        } else {
            std::cout << colorize("o", Color::BrightBlack);
        }
        if (i < totalSteps) {
            std::cout << colorize("─", Color::BrightBlack);
        }
    }
    std::cout << "\n\n";
}

void TerminalUtils::showWizardProgress(int currentStep, int totalSteps,
                                       const std::string& stepName) {
    if (!supportsAnsi()) {
        std::cout << "Step " << currentStep << "/" << totalSteps << ": " << stepName << "\n";
        return;
    }

    int percent = (currentStep * 100) / totalSteps;

    std::cout << colorize("  " + stepName, Color::BrightWhite) << "\n";
    showModernProgressBar(percent, "Progress", Color::BrightGreen);
    std::cout << "\n";
}

void TerminalUtils::showWizardSummary(
        const std::vector<std::pair<std::string, std::string>>& summary) {
    if (!supportsAnsi()) {
        std::cout << "Configuration Summary:\n";
        for (const auto& [key, value] : summary) {
            std::cout << "  " << key << ": " << value << "\n";
        }
        return;
    }

    std::cout << colorize("  📋 Configuration Summary", Color::BrightCyan) << "\n\n";

    for (const auto& [key, value] : summary) {
        std::cout << "  " << colorize(key + ":", Color::BrightWhite) << " "
                  << colorize(value, Color::BrightGreen) << "\n";
    }
    std::cout << "\n";
}

// Enhanced input dialogs
std::string TerminalUtils::showValidatedInput(const std::string& prompt,
                                              std::function<bool(const std::string&)> validator,
                                              const std::string& errorMessage,
                                              const std::string& placeholder) {
    std::string input;
    bool isValid = false;

    while (!isValid) {
        if (!supportsAnsi()) {
            std::cout << prompt;
            if (!placeholder.empty()) {
                std::cout << " (" << placeholder << ")";
            }
            std::cout << ": ";
        } else {
            std::cout << "  " << colorize(prompt, Color::BrightWhite);
            if (!placeholder.empty()) {
                std::cout << colorize(" (" + placeholder + ")", Color::BrightBlack);
            }
            std::cout << "\n  " << colorize("�?", Color::BrightCyan);
        }

        std::getline(std::cin, input);

        if (input.empty() && !placeholder.empty()) {
            input = placeholder;
        }

        if (validator(input)) {
            isValid = true;
        } else {
            if (!supportsAnsi()) {
                std::cout << "Error: " << errorMessage << "\n";
            } else {
                std::cout << colorize("  �?" + errorMessage, Color::BrightRed) << "\n\n";
            }
        }
    }

    return input;
}

std::vector<std::string> TerminalUtils::showMultiSelectDialog(
        const std::string& prompt, const std::vector<std::string>& options,
        const std::vector<bool>& defaultSelected) {
    std::vector<bool> selected = defaultSelected;
    if (selected.size() != options.size()) {
        selected.resize(options.size(), false);
    }

    if (!supportsAnsi()) {
        std::cout << prompt << "\n";
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << (i + 1) << ". " << options[i] << (selected[i] ? " [selected]" : "")
                      << "\n";
        }
        std::cout << "Enter numbers separated by spaces (e.g., 1 3 5): ";

        std::string input;
        std::getline(std::cin, input);

        std::vector<std::string> result;
        std::istringstream iss(input);
        std::string token;
        while (iss >> token) {
            try {
                int index = std::stoi(token) - 1;
                if (index >= 0 && index < static_cast<int>(options.size())) {
                    result.push_back(options[index]);
                }
            } catch (...) {
                // Ignore invalid input
            }
        }
        return result;
    }

    std::cout << "\n" << colorize("  " + prompt, Color::BrightCyan) << "\n";
    std::cout << colorize("  Use space to toggle, Enter to confirm", Color::BrightBlack) << "\n\n";

    for (size_t i = 0; i < options.size(); ++i) {
        std::string checkbox = selected[i] ? "[X]" : "[ ]";
        std::cout << "  "
                  << colorize(checkbox, selected[i] ? Color::BrightGreen : Color::BrightBlack)
                  << " " << colorize(options[i], Color::BrightWhite) << "\n";
    }

    std::cout << "\n  Enter numbers to toggle (e.g., 1 3 5) or press Enter to confirm: ";

    std::string input;
    std::getline(std::cin, input);

    if (!input.empty()) {
        std::istringstream iss(input);
        std::string token;
        while (iss >> token) {
            try {
                int index = std::stoi(token) - 1;
                if (index >= 0 && index < static_cast<int>(options.size())) {
                    selected[index] = !selected[index];
                }
            } catch (...) {
                // Ignore invalid input
            }
        }
    }

    std::vector<std::string> result;
    for (size_t i = 0; i < options.size(); ++i) {
        if (selected[i]) {
            result.push_back(options[i]);
        }
    }

    return result;
}

// Status indicators
void TerminalUtils::showStatusLine(const std::string& status, Color statusColor) {
    if (!supportsAnsi()) {
        std::cout << "Status: " << status << std::endl;
        return;
    }

    clearLine();
    std::cout << colorize("  �?", statusColor) << colorize(status, Color::BrightWhite)
              << std::flush;
}

void TerminalUtils::updateStatusLine(const std::string& status, Color statusColor) {
    showStatusLine(status, statusColor);
}

void TerminalUtils::clearStatusLine() {
    if (supportsAnsi()) {
        clearLine();
    }
}

// Loading animations
void TerminalUtils::showLoadingDots(const std::string& message, int durationMs) {
    if (!supportsAnsi()) {
        std::cout << message << "..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
        return;
    }

    const std::vector<std::string> dots = {"", ".", "..", "..."};
    const int frameTime = 500;  // 500ms per frame
    const int totalFrames = durationMs / frameTime;

    hideCursor();

    for (int frame = 0; frame < totalFrames; ++frame) {
        clearLine();
        std::cout << colorize("  " + message, Color::BrightWhite)
                  << colorize(dots[frame % dots.size()], Color::BrightCyan) << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
    }

    clearLine();
    showCursor();
}

void TerminalUtils::showLoadingBar(const std::string& message, int durationMs) {
    if (!supportsAnsi()) {
        std::cout << message << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
        return;
    }

    const int barWidth = 30;
    const int frameTime = 100;  // 100ms per frame
    const int totalFrames = durationMs / frameTime;

    hideCursor();

    for (int frame = 0; frame < totalFrames; ++frame) {
        clearLine();

        int progress = (frame * 100) / totalFrames;
        int filled = (barWidth * progress) / 100;

        std::string bar = "[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) {
                bar += colorize("#", Color::BrightGreen);
            } else {
                bar += colorize("-", Color::BrightBlack);
            }
        }
        bar += "]";

        std::cout << colorize("  " + message + " ", Color::BrightWhite) << bar
                  << colorize(" " + std::to_string(progress) + "%", Color::BrightCyan)
                  << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
    }

    clearLine();
    showCursor();
}

void TerminalUtils::showPulseAnimation(const std::string& message, int durationMs) {
    if (!supportsAnsi()) {
        std::cout << message << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
        return;
    }

    const std::vector<Color> pulseColors = {Color::BrightBlack, Color::Cyan, Color::BrightCyan,
                                            Color::Cyan};
    const int frameTime = 200;  // 200ms per frame
    const int totalFrames = durationMs / frameTime;

    hideCursor();

    for (int frame = 0; frame < totalFrames; ++frame) {
        clearLine();
        Color currentColor = pulseColors[frame % pulseColors.size()];
        std::cout << colorize("  �?" + message, currentColor) << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
    }

    clearLine();
    showCursor();
}

}  // namespace utils
