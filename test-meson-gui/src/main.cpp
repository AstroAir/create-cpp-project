#include <QApplication>
#include "test-meson-gui/logging.h"

int main(int argc, char* argv[]) {
    // 初始化日志系�?
    test-meson-gui::Logging::init("logs/app.log");

    // 创建应用实例
    QApplication qtApp(argc, argv);
    test-meson-gui::Application app;

    // 显示主窗�?
    if (!app.initialize()) {
        SPDLOG_ERROR("应用初始化失�?);
        return 1;
    }

    // 运行应用主循�?
    SPDLOG_INFO("应用启动成功");
    int result = qtApp.exec();
    SPDLOG_INFO("应用退出，返回�? {}", result);

    return result;
}
