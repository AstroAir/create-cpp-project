/**
 * @file test_logger.cpp
 * @brief Tests for Logger class
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include "../src/utils/logger.h"
#include <filesystem>

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        utils::Logger::init();
    }
    
    void TearDown() override {
        // Clean up any log files
        if (std::filesystem::exists("test.log")) {
            std::filesystem::remove("test.log");
        }
    }
};

TEST_F(LoggerTest, SetLevel) {
    utils::Logger::set_level(utils::LogLevel::Debug);
    EXPECT_EQ(utils::Logger::get_level(), utils::LogLevel::Debug);
    
    utils::Logger::set_level("info");
    EXPECT_EQ(utils::Logger::get_level(), utils::LogLevel::Info);
}

TEST_F(LoggerTest, IsEnabled) {
    utils::Logger::set_level(utils::LogLevel::Info);
    
    EXPECT_FALSE(utils::Logger::is_enabled(utils::LogLevel::Debug));
    EXPECT_TRUE(utils::Logger::is_enabled(utils::LogLevel::Info));
    EXPECT_TRUE(utils::Logger::is_enabled(utils::LogLevel::Warning));
    EXPECT_TRUE(utils::Logger::is_enabled(utils::LogLevel::Error));
}

TEST_F(LoggerTest, BasicLogging) {
    // These tests just verify the logging functions don't crash
    utils::Logger::info("Test info message");
    utils::Logger::warn("Test warning message");
    utils::Logger::error("Test error message");
    utils::Logger::debug("Test debug message");
}

TEST_F(LoggerTest, FormattedLogging) {
    // Test formatted logging
    utils::Logger::info("Test with number: {}", 42);
    utils::Logger::warn("Test with string: {}", "hello");
}
