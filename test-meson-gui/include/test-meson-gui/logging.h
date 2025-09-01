#pragma once
#ifndef TEST-MESON-GUI_LOGGING_H
#define TEST-MESON-GUI_LOGGING_H

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

// 定义使用spdlog的宏，方便在项目中调�?
#define SPDLOG_TRACE(...) SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)

namespace test-meson-gui {

class Logging {
public:
    // 初始化日志系�?
    static bool init(const std::string& logFilePath,
                   spdlog::level::level_enum level = spdlog::level::info);

    // 关闭日志系统
    static void shutdown();

    // 设置日志级别
    static void setLevel(spdlog::level::level_enum level);

    // 获取当前日志级别
    static spdlog::level::level_enum getLevel();

    // 获取格式化时间戳
    static std::string getFormattedTimestamp();

private:
    // 是否已初始化
    static bool initialized_;

    // 日志文件路径
    static std::string logFilePath_;

    // 当前日志级别
    static spdlog::level::level_enum level_;

    // 创建日志目录
    static bool createLogDirectory(const std::string& path);
};

} // namespace test-meson-gui

#endif // TEST-MESON-GUI_LOGGING_H
