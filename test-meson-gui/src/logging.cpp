#include "test-meson-gui/logging.h"
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace test-meson-gui {

// 静态成员初始化
bool Logging::initialized_ = false;
std::string Logging::logFilePath_ = "logs/app.log";
spdlog::level::level_enum Logging::level_ = spdlog::level::info;

bool Logging::init(const std::string& logFilePath, spdlog::level::level_enum level) {
    if (initialized_) {
        return true; // 已经初始化过
    }

    try {
        logFilePath_ = logFilePath;
        level_ = level;

        // 创建日志目录
        if (!createLogDirectory(logFilePath_)) {
            // 如果创建目录失败，回退到控制台日志
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto logger = std::make_shared<spdlog::logger>("console_logger", console_sink);
            logger->set_level(level_);
            spdlog::set_default_logger(logger);

            spdlog::warn("无法创建日志目录，仅使用控制台输�?);
            initialized_ = true;
            return false;
        }

        // 创建一个旋转文件日志，最�?MB，保�?个备�?
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFilePath_, 5 * 1024 * 1024, 3);

        // 同时输出到控制台
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // 创建带有两个接收器的记录�?
        std::vector<spdlog::sink_ptr> sinks {rotating_sink, console_sink};
        auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

        // 设置记录格式
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        // 设置日志级别
        logger->set_level(level_);

        // 设置为默认记录器
        spdlog::set_default_logger(logger);

        // 记录初始化信�?
        spdlog::info("日志系统初始化成功，级别: {}, 路径: {}",
                   spdlog::level::to_string_view(level_), logFilePath_);

        initialized_ = true;
        return true;
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "日志初始化失�? " << ex.what() << std::endl;
        return false;
    }
}

void Logging::shutdown() {
    if (initialized_) {
        spdlog::info("关闭日志系统");
        spdlog::shutdown();
        initialized_ = false;
    }
}

void Logging::setLevel(spdlog::level::level_enum level) {
    level_ = level;
    if (initialized_) {
        spdlog::set_level(level_);
        spdlog::info("日志级别设置�? {}", spdlog::level::to_string_view(level_));
    }
}

spdlog::level::level_enum Logging::getLevel() {
    return level_;
}

std::string Logging::getFormattedTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

bool Logging::createLogDirectory(const std::string& path) {
    try {
        std::filesystem::path p(path);
        auto dir = p.parent_path();

        if (!dir.empty() && !std::filesystem::exists(dir)) {
            return std::filesystem::create_directories(dir);
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "创建日志目录失败: " << e.what() << std::endl;
        return false;
    }
}

} // namespace test-meson-gui
