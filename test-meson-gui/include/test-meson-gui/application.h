#pragma once
#ifndef TEST-MESON-GUI_APPLICATION_H
#define TEST-MESON-GUI_APPLICATION_H

#include "main_window.h"
#include <QObject>
#include <memory>
#include <spdlog/spdlog.h>

namespace test-meson-gui {

class Application : public QObject {
    Q_OBJECT

public:
    Application();
    ~Application() override;

    // 初始化应用程�?
    bool initialize();

    // 获取主窗�?
    MainWindow* mainWindow() { return mainWindow_.get(); }

private:
    // 主窗�?
    std::unique_ptr<MainWindow> mainWindow_;

    // 加载应用程序配置
    bool loadSettings();

    // 保存应用程序配置
    bool saveSettings();
};

} // namespace test-meson-gui

#endif // TEST-MESON-GUI_APPLICATION_H
