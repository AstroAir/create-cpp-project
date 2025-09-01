#pragma once
#ifndef TEST-MESON-GUI_MAIN_WINDOW_H
#define TEST-MESON-GUI_MAIN_WINDOW_H

#include <QMainWindow>
#include <memory>
#include <spdlog/spdlog.h>

// 前向声明
namespace Ui {
class MainWindow;
}

namespace test-meson-gui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // 初始化窗�?
    bool initialize();

private slots:
    // 槽函�?
    void onActionExit();
    void onActionAbout();

private:
    // UI设计器生成的UI�?
    std::unique_ptr<Ui::MainWindow> ui;

    // 初始化菜�?
    void setupMenus();

    // 初始化状态栏
    void setupStatusBar();

    // 连接信号和槽
    void connectSignals();
};

} // namespace test-meson-gui

#endif // TEST-MESON-GUI_MAIN_WINDOW_H
