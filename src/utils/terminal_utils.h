#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace utils {
enum class Color {
  Reset,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  BrightBlack,
  BrightRed,
  BrightGreen,
  BrightYellow,
  BrightBlue,
  BrightMagenta,
  BrightCyan,
  BrightWhite
};

class TerminalUtils {
public:
  // 检查终端是否支持ANSI转义序列
  static bool supportsAnsi();

  // 终端颜色输出
  static std::string colorize(const std::string &text, Color foreground,
                              Color background = Color::Reset);

  // 在当前位置显示进度条
  static void showProgressBar(int percent, int width = 50);

  // 显示旋转器（例如：|/-\）
  static void showSpinner(int state);

  // 清除当前行
  static void clearLine();

  // 移动光标到行首
  static void moveCursorToLineStart();

  // 终端宽度
  static int getTerminalWidth();

  // 居中显示文本
  static std::string centerText(const std::string &text, int width = 0);

  // 显示有边框的框
  static void showBox(const std::vector<std::string> &lines);

  // 显示通知/成功/错误消息
  static void showSuccess(const std::string &message);
  static void showError(const std::string &message);
  static void showInfo(const std::string &message);
  static void showWarning(const std::string &message);

private:
  static std::string getAnsiColorCode(Color color, bool isBackground = false);
};
} // namespace utils