#include "test-meson-project/application.h"

namespace test-meson-project {

Application::Application()
    : QObject(nullptr)
{
    SPDLOG_DEBUG("Application构造函�?);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application析构函数");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("初始化应用程�?);

    // 加载设置
    if (!loadSettings()) {
        SPDLOG_WARN("无法加载应用程序设置");
    }

    // 创建并初始化主窗�?
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("初始化主窗口失败");
        return false;
    }

    // 显示主窗�?
    mainWindow_->show();

    SPDLOG_INFO("应用程序初始化完�?);
    return true;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("加载应用程序设置");
    // TODO: 实现设置加载逻辑
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("保存应用程序设置");
    // TODO: 实现设置保存逻辑
    return true;
}

} // namespace test-meson-project
