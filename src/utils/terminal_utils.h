#pragma once

#include <functional>
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

enum class TextStyle {
  Bold,
  Dim,
  Italic,
  Underline,
  Blink,
  Reverse,
  Hidden,
  Strike
};

enum class BorderStyle { Single, Double, Rounded, Bold, Ascii };

struct TableCell {
  std::string text;
  Color foreground = Color::Reset;
  Color background = Color::Reset;
  std::vector<TextStyle> styles;
  bool centered = false;

  TableCell(const std::string &txt) : text(txt) {}

  TableCell(const std::string &txt, Color fg) : text(txt), foreground(fg) {}

  TableCell(const std::string &txt, Color fg, const std::vector<TextStyle> &st)
      : text(txt), foreground(fg), styles(st) {}

  TableCell(const std::string &txt, Color fg, Color bg,
            const std::vector<TextStyle> &st = {})
      : text(txt), foreground(fg), background(bg), styles(st) {}
};

class TerminalUtils {
public:
  // 检查终端是否支持ANSI转义序列
  static bool supportsAnsi();

  // 终端颜色输出
  static std::string colorize(const std::string &text, Color foreground,
                              Color background = Color::Reset);

  // 文本样式设置
  static std::string stylize(const std::string &text,
                             const std::vector<TextStyle> &styles);

  // 组合颜色和样式
  static std::string colorAndStyle(const std::string &text, Color foreground,
                                   const std::vector<TextStyle> &styles = {},
                                   Color background = Color::Reset);

  // 清屏
  static void clearScreen();

  // 在当前位置显示进度条
  static void showProgressBar(int percent, int width = 50);

  // 彩色进度条
  static void showProgressBar(int percent, int width, Color fillColor,
                              Color emptyColor);

  // 动画进度条
  static void runAnimatedProgress(int durationMs,
                                  const std::function<int(int)> &progressFunc,
                                  int width = 50,
                                  Color fillColor = Color::Green,
                                  Color emptyColor = Color::BrightBlack);

  // 显示旋转器（例如：|/-\）
  static void showSpinner(int state);

  // 运行旋转器动画
  static void runSpinner(int durationMs, const std::string &message = "",
                         int framesPerSecond = 10);

  // 清除当前行
  static void clearLine();

  // 移动光标到行首
  static void moveCursorToLineStart();

  // 移动光标到指定位置
  static void moveCursor(int row, int col);

  // 保存/恢复光标位置
  static void saveCursorPosition();
  static void restoreCursorPosition();

  // 显示/隐藏光标
  static void hideCursor();
  static void showCursor();

  // 终端宽度
  static int getTerminalWidth();

  // 终端高度
  static int getTerminalHeight();

  // 终端大小
  static std::pair<int, int> getTerminalSize();

  // 居中显示文本
  static std::string centerText(const std::string &text, int width = 0);

  // 右对齐文本
  static std::string rightAlign(const std::string &text, int width = 0);

  // 文本截断
  static std::string truncate(const std::string &text, int maxLength,
                              bool addEllipsis = true);

  // 文本换行
  static std::vector<std::string> wrapText(const std::string &text,
                                           int width = 0);

  // 获取边框字符
  static std::vector<std::string> getBorderChars(BorderStyle style);

  // 显示有边框的框
  static void showBox(const std::vector<std::string> &lines);

  // 显示自定义样式和颜色的框
  static void showBox(const std::vector<std::string> &lines,
                      BorderStyle style = BorderStyle::Single,
                      Color borderColor = Color::Reset,
                      Color textColor = Color::Reset, std::string title = "");

  // 显示表格
  static void showTable(const std::vector<std::vector<TableCell>> &rows,
                        bool hasHeader = true,
                        BorderStyle style = BorderStyle::Single,
                        Color borderColor = Color::Reset);

  // 显示通知/成功/错误消息
  static void showSuccess(const std::string &message);
  static void showError(const std::string &message);
  static void showInfo(const std::string &message);
  static void showWarning(const std::string &message);
  static void showDebug(const std::string &message);

  // 显示选择菜单
  static int showMenu(const std::vector<std::string> &options,
                      const std::string &prompt = "请选择一个选项：",
                      Color highlightColor = Color::Cyan);

  // 处理终端大小变化
  static void handleTerminalResize(std::function<void()> redrawFunc);

private:
  static bool lastWasNewline;
  static std::string getAnsiColorCode(Color color, bool isBackground = false);
  static std::string getAnsiStyleCode(TextStyle style);
};
} // namespace utils