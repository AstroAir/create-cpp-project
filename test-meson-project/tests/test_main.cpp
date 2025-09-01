#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "test-meson-project/logging.h"
#include <memory>

// 初始化日志系�?
class LoggingFixture {
public:
    LoggingFixture() {
        test-meson-project::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("测试开�?);
    }

    ~LoggingFixture() {
        SPDLOG_INFO("测试结束");
        test-meson-project::Logging::shutdown();
    }
};

TEST_CASE_FIXTURE(LoggingFixture, "测试Logging�?) {
    SUBCASE("测试日志级别") {
        CHECK(test-meson-project::Logging::getLevel() == spdlog::level::debug);

        // 测试日志级别设置
        test-meson-project::Logging::setLevel(spdlog::level::info);
        CHECK(test-meson-project::Logging::getLevel() == spdlog::level::info);
    }

    SUBCASE("测试格式化时间戳") {
        auto timestamp = test-meson-project::Logging::getFormattedTimestamp();
        CHECK(!timestamp.empty());
    }
}
