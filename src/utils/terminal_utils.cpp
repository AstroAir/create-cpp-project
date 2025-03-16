#include "terminal_utils.h"

#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace utils {

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
  return SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
  // Most UNIX terminals support ANSI
  return isatty(STDOUT_FILENO);
#endif
}

std::string TerminalUtils::getAnsiColorCode(Color color, bool isBackground) {
  if (!supportsAnsi())
    return "";

  int baseCode = isBackground ? 40 : 30;
  int brightModifier = 0;

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

std::string TerminalUtils::colorize(const std::string &text, Color foreground,
                                    Color background) {
  if (!supportsAnsi())
    return text;

  std::string result = getAnsiColorCode(foreground);
  if (background != Color::Reset) {
    result += getAnsiColorCode(background, true);
  }

  result += text + getAnsiColorCode(Color::Reset);
  return result;
}

void TerminalUtils::showProgressBar(int percent, int width) {
  if (!supportsAnsi()) {
    // 简单输出百分比
    std::cout << "Progress: " << percent << "%" << std::endl;
    return;
  }

  // 确保百分比在合理范围内
  percent = std::max(0, std::min(100, percent));

  std::string bar;
  bar.reserve(width + 10);

  int filledWidth = width * percent / 100;

  // 构建进度条
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

  // 添加百分比
  bar += "] " + std::to_string(percent) + "%";

  // 清除当前行并显示进度条
  clearLine();
  std::cout << bar << std::flush;
}

void TerminalUtils::showSpinner(int state) {
  if (!supportsAnsi())
    return;

  static const char spinner[] = {'|', '/', '-', '\\'};
  clearLine();
  std::cout << spinner[state % 4] << std::flush;
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

int TerminalUtils::getTerminalWidth() {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int columns = 80; // 默认宽度

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

  return columns;
#else
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
#endif
}

std::string TerminalUtils::centerText(const std::string &text, int width) {
  if (width <= 0) {
    width = getTerminalWidth();
  }

  int textWidth = static_cast<int>(text.length());
  if (textWidth >= width)
    return text;

  int padding = (width - textWidth) / 2;
  return std::string(padding, ' ') + text;
}

void TerminalUtils::showBox(const std::vector<std::string> &lines) {
  if (lines.empty())
    return;

  // 找出最长行的长度
  int maxLength = 0;
  for (const auto &line : lines) {
    maxLength = std::max(maxLength, static_cast<int>(line.length()));
  }

  // 上边框
  std::cout << "┌" << std::string(maxLength + 2, '-') << "┐" << std::endl;

  // 内容
  for (const auto &line : lines) {
    std::cout << "│ " << line << std::string(maxLength - line.length(), ' ')
              << " │" << std::endl;
  }

  // 下边框
  std::cout << "└" << std::string(maxLength + 2, '-') << "┘" << std::endl;
}

void TerminalUtils::showSuccess(const std::string &message) {
  std::cout << colorize("✅ " + message, Color::BrightGreen) << std::endl;
}

void TerminalUtils::showError(const std::string &message) {
  std::cout << colorize("❌ " + message, Color::BrightRed) << std::endl;
}

void TerminalUtils::showInfo(const std::string &message) {
  std::cout << colorize("ℹ️ " + message, Color::BrightBlue) << std::endl;
}

void TerminalUtils::showWarning(const std::string &message) {
  std::cout << colorize("⚠️ " + message, Color::BrightYellow) << std::endl;
}

} // namespace utils