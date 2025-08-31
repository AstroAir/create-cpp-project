/**
 * @file logger.h
 * @brief Logging utility for the file processor application
 * 
 * Provides a simple but effective logging system with multiple levels,
 * file output, and formatting capabilities.
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>

namespace utils {

/**
 * @brief Log level enumeration
 */
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Off = 5
};

/**
 * @brief Simple logging utility class
 * 
 * This class provides thread-safe logging with configurable levels,
 * file output, and formatted messages. It's designed to be lightweight
 * and easy to use throughout the application.
 */
class Logger {
public:
    /**
     * @brief Initialize the logger with default settings
     */
    static void init();
    
    /**
     * @brief Set the minimum log level
     * @param level Minimum level to log
     */
    static void set_level(LogLevel level);
    
    /**
     * @brief Set the minimum log level from string
     * @param level_str Level string (trace, debug, info, warn, error)
     */
    static void set_level(const std::string& level_str);
    
    /**
     * @brief Set output file for logging
     * @param filename Path to log file
     */
    static void set_file(const std::string& filename);
    
    /**
     * @brief Enable or disable console output
     * @param enable True to enable console output
     */
    static void set_console_output(bool enable);
    
    /**
     * @brief Log a trace message
     * @param message Message to log
     */
    static void trace(const std::string& message);
    
    /**
     * @brief Log a debug message
     * @param message Message to log
     */
    static void debug(const std::string& message);
    
    /**
     * @brief Log an info message
     * @param message Message to log
     */
    static void info(const std::string& message);
    
    /**
     * @brief Log a warning message
     * @param message Message to log
     */
    static void warn(const std::string& message);
    
    /**
     * @brief Log an error message
     * @param message Message to log
     */
    static void error(const std::string& message);
    
    /**
     * @brief Log a formatted trace message
     * @tparam Args Argument types for formatting
     * @param format Format string
     * @param args Arguments for formatting
     */
    template<typename... Args>
    static void trace(const std::string& format, Args&&... args) {
        log(LogLevel::Trace, format_message(format, std::forward<Args>(args)...));
    }
    
    /**
     * @brief Log a formatted debug message
     * @tparam Args Argument types for formatting
     * @param format Format string
     * @param args Arguments for formatting
     */
    template<typename... Args>
    static void debug(const std::string& format, Args&&... args) {
        log(LogLevel::Debug, format_message(format, std::forward<Args>(args)...));
    }
    
    /**
     * @brief Log a formatted info message
     * @tparam Args Argument types for formatting
     * @param format Format string
     * @param args Arguments for formatting
     */
    template<typename... Args>
    static void info(const std::string& format, Args&&... args) {
        log(LogLevel::Info, format_message(format, std::forward<Args>(args)...));
    }
    
    /**
     * @brief Log a formatted warning message
     * @tparam Args Argument types for formatting
     * @param format Format string
     * @param args Arguments for formatting
     */
    template<typename... Args>
    static void warn(const std::string& format, Args&&... args) {
        log(LogLevel::Warning, format_message(format, std::forward<Args>(args)...));
    }
    
    /**
     * @brief Log a formatted error message
     * @tparam Args Argument types for formatting
     * @param format Format string
     * @param args Arguments for formatting
     */
    template<typename... Args>
    static void error(const std::string& format, Args&&... args) {
        log(LogLevel::Error, format_message(format, std::forward<Args>(args)...));
    }
    
    /**
     * @brief Flush all log outputs
     */
    static void flush();
    
    /**
     * @brief Get current log level
     * @return Current minimum log level
     */
    static LogLevel get_level();
    
    /**
     * @brief Check if a log level is enabled
     * @param level Level to check
     * @return True if level is enabled
     */
    static bool is_enabled(LogLevel level);

private:
    static LogLevel current_level_;
    static std::unique_ptr<std::ofstream> file_stream_;
    static bool console_output_;
    static std::mutex mutex_;
    
    /**
     * @brief Internal logging function
     * @param level Log level
     * @param message Message to log
     */
    static void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Get string representation of log level
     * @param level Log level
     * @return String representation
     */
    static std::string level_to_string(LogLevel level);
    
    /**
     * @brief Get current timestamp string
     * @return Formatted timestamp
     */
    static std::string get_timestamp();
    
    /**
     * @brief Format message with arguments
     * @tparam Args Argument types
     * @param format Format string
     * @param args Arguments
     * @return Formatted string
     */
    template<typename... Args>
    static std::string format_message(const std::string& format, Args&&... args) {
        // Simple string replacement formatting
        // In a real implementation, you might use fmt library or similar
        std::ostringstream oss;
        format_impl(oss, format, std::forward<Args>(args)...);
        return oss.str();
    }
    
    /**
     * @brief Implementation helper for format_message
     */
    template<typename T>
    static void format_impl(std::ostringstream& oss, const std::string& format, T&& value) {
        auto pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << value << format.substr(pos + 2);
        } else {
            oss << format;
        }
    }
    
    /**
     * @brief Implementation helper for format_message with multiple arguments
     */
    template<typename T, typename... Args>
    static void format_impl(std::ostringstream& oss, const std::string& format, T&& value, Args&&... args) {
        auto pos = format.find("{}");
        if (pos != std::string::npos) {
            std::string remaining = format.substr(0, pos) + std::to_string(value) + format.substr(pos + 2);
            format_impl(oss, remaining, std::forward<Args>(args)...);
        } else {
            oss << format;
        }
    }
    
    /**
     * @brief Convert string to log level
     * @param level_str String representation
     * @return Log level
     */
    static LogLevel string_to_level(const std::string& level_str);
};

/**
 * @brief RAII class for scoped logging
 * 
 * Automatically logs entry and exit of a scope, useful for debugging
 * function calls and performance monitoring.
 */
class ScopedLogger {
public:
    /**
     * @brief Constructor - logs entry
     * @param scope_name Name of the scope
     * @param level Log level to use
     */
    explicit ScopedLogger(const std::string& scope_name, LogLevel level = LogLevel::Debug);
    
    /**
     * @brief Destructor - logs exit with duration
     */
    ~ScopedLogger();
    
    /**
     * @brief Log a message within the scope
     * @param message Message to log
     */
    void log(const std::string& message);

private:
    std::string scope_name_;
    LogLevel level_;
    std::chrono::steady_clock::time_point start_time_;
};

/**
 * @brief Macro for easy scoped logging
 */
#define SCOPED_LOG(name) utils::ScopedLogger _scoped_logger(name)

/**
 * @brief Macro for scoped logging with custom level
 */
#define SCOPED_LOG_LEVEL(name, level) utils::ScopedLogger _scoped_logger(name, level)

} // namespace utils
