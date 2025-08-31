/**
 * @file logger.cpp
 * @brief Implementation of the Logger utility class
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include "logger.h"
#include <algorithm>
#include <cctype>

namespace utils {

// Static member definitions
LogLevel Logger::current_level_ = LogLevel::Info;
std::unique_ptr<std::ofstream> Logger::file_stream_ = nullptr;
bool Logger::console_output_ = true;
std::mutex Logger::mutex_;

void Logger::init() {
    std::lock_guard<std::mutex> lock(mutex_);
    current_level_ = LogLevel::Info;
    console_output_ = true;
    file_stream_.reset();
}

void Logger::set_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    current_level_ = level;
}

void Logger::set_level(const std::string& level_str) {
    set_level(string_to_level(level_str));
}

void Logger::set_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    file_stream_ = std::make_unique<std::ofstream>(filename, std::ios::app);
    if (!file_stream_->is_open()) {
        file_stream_.reset();
        throw std::runtime_error("Cannot open log file: " + filename);
    }
}

void Logger::set_console_output(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    console_output_ = enable;
}

void Logger::trace(const std::string& message) {
    log(LogLevel::Trace, message);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::Debug, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::Info, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::Error, message);
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_stream_) {
        file_stream_->flush();
    }
    if (console_output_) {
        std::cout.flush();
        std::cerr.flush();
    }
}

LogLevel Logger::get_level() {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_level_;
}

bool Logger::is_enabled(LogLevel level) {
    return level >= current_level_;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!is_enabled(level)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string timestamp = get_timestamp();
    std::string level_str = level_to_string(level);
    std::string formatted_message = "[" + timestamp + "] [" + level_str + "] " + message;
    
    // Output to console
    if (console_output_) {
        if (level >= LogLevel::Warning) {
            std::cerr << formatted_message << std::endl;
        } else {
            std::cout << formatted_message << std::endl;
        }
    }
    
    // Output to file
    if (file_stream_ && file_stream_->is_open()) {
        *file_stream_ << formatted_message << std::endl;
    }
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:   return "TRACE";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Off:     return "OFF  ";
        default:                return "UNKN ";
    }
}

std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

LogLevel Logger::string_to_level(const std::string& level_str) {
    std::string lower_str = level_str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    if (lower_str == "trace") return LogLevel::Trace;
    if (lower_str == "debug") return LogLevel::Debug;
    if (lower_str == "info") return LogLevel::Info;
    if (lower_str == "warn" || lower_str == "warning") return LogLevel::Warning;
    if (lower_str == "error") return LogLevel::Error;
    if (lower_str == "off") return LogLevel::Off;
    
    // Default to Info if unknown
    return LogLevel::Info;
}

// ScopedLogger implementation
ScopedLogger::ScopedLogger(const std::string& scope_name, LogLevel level)
    : scope_name_(scope_name), level_(level), start_time_(std::chrono::steady_clock::now()) {
    switch (level_) {
        case LogLevel::Trace: Logger::trace("Entering " + scope_name_); break;
        case LogLevel::Debug: Logger::debug("Entering " + scope_name_); break;
        case LogLevel::Info: Logger::info("Entering " + scope_name_); break;
        case LogLevel::Warning: Logger::warn("Entering " + scope_name_); break;
        case LogLevel::Error: Logger::error("Entering " + scope_name_); break;
        default: break;
    }
}

ScopedLogger::~ScopedLogger() {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
    std::string message = "Exiting " + scope_name_ + " (duration: " + std::to_string(duration.count()) + "ms)";

    switch (level_) {
        case LogLevel::Trace: Logger::trace(message); break;
        case LogLevel::Debug: Logger::debug(message); break;
        case LogLevel::Info: Logger::info(message); break;
        case LogLevel::Warning: Logger::warn(message); break;
        case LogLevel::Error: Logger::error(message); break;
        default: break;
    }
}

void ScopedLogger::log(const std::string& message) {
    std::string scoped_message = "[" + scope_name_ + "] " + message;

    switch (level_) {
        case LogLevel::Trace: Logger::trace(scoped_message); break;
        case LogLevel::Debug: Logger::debug(scoped_message); break;
        case LogLevel::Info: Logger::info(scoped_message); break;
        case LogLevel::Warning: Logger::warn(scoped_message); break;
        case LogLevel::Error: Logger::error(scoped_message); break;
        default: break;
    }
}

} // namespace utils
