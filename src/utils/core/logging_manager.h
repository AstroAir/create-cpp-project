#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>

namespace utils {

// Log levels
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string category;
    std::string file;
    int line;
    std::string function;
    std::thread::id threadId;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
};

// Log destination interface
class LogDestination {
public:
    virtual ~LogDestination() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
    virtual std::string getName() const = 0;
};

// Console log destination
class ConsoleDestination : public LogDestination {
public:
    ConsoleDestination(bool useColors = true);
    void write(const LogEntry& entry) override;
    void flush() override;
    void close() override;
    std::string getName() const override { return "Console"; }

private:
    bool useColors_;
    std::mutex mutex_;
    std::string formatEntry(const LogEntry& entry);
    std::string getColorForLevel(LogLevel level);
};

// File log destination
class FileDestination : public LogDestination {
public:
    FileDestination(const std::filesystem::path& filePath, 
                   bool rotateDaily = false, 
                   size_t maxFileSizeMB = 100);
    ~FileDestination();
    void write(const LogEntry& entry) override;
    void flush() override;
    void close() override;
    std::string getName() const override { return "File: " + filePath_.string(); }

private:
    std::filesystem::path filePath_;
    std::unique_ptr<std::ofstream> fileStream_;
    bool rotateDaily_;
    size_t maxFileSizeMB_;
    size_t currentFileSizeBytes_;
    std::string currentDateString_;
    std::mutex mutex_;
    
    void rotateIfNeeded();
    void createRotatedFileName();
    std::string formatEntry(const LogEntry& entry);
};

// Network log destination (for centralized logging)
class NetworkDestination : public LogDestination {
public:
    NetworkDestination(const std::string& host, int port, const std::string& protocol = "tcp");
    ~NetworkDestination();
    void write(const LogEntry& entry) override;
    void flush() override;
    void close() override;
    std::string getName() const override { return "Network: " + host_ + ":" + std::to_string(port_); }

private:
    std::string host_;
    int port_;
    std::string protocol_;
    std::mutex mutex_;
    bool connected_;
    
    bool connect();
    void disconnect();
    std::string formatEntry(const LogEntry& entry);
};

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
};

// Standard log formatter
class StandardFormatter : public LogFormatter {
public:
    StandardFormatter(const std::string& pattern = "[{timestamp}] [{level}] [{category}] {message}");
    std::string format(const LogEntry& entry) override;

private:
    std::string pattern_;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    std::string levelToString(LogLevel level);
};

// JSON log formatter
class JsonFormatter : public LogFormatter {
public:
    JsonFormatter(bool prettyPrint = false);
    std::string format(const LogEntry& entry) override;

private:
    bool prettyPrint_;
};

// Performance monitoring
struct PerformanceMetrics {
    size_t totalLogs = 0;
    size_t logsPerLevel[7] = {0}; // One for each LogLevel
    std::chrono::milliseconds totalLoggingTime{0};
    std::chrono::milliseconds avgLoggingTime{0};
    size_t droppedLogs = 0;
    size_t queueSize = 0;
    size_t maxQueueSize = 0;
};

// Async logging manager
class LoggingManager {
public:
    static LoggingManager& getInstance();
    
    // Configuration
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void setGlobalLogLevel(LogLevel level);
    void setCategoryLogLevel(const std::string& category, LogLevel level);
    
    // Destinations management
    void addDestination(std::unique_ptr<LogDestination> destination);
    void removeDestination(const std::string& name);
    void clearDestinations();
    std::vector<std::string> getDestinationNames() const;
    
    // Formatters
    void setFormatter(std::unique_ptr<LogFormatter> formatter);
    void setCategoryFormatter(const std::string& category, std::unique_ptr<LogFormatter> formatter);
    
    // Logging methods
    void log(LogLevel level, const std::string& category, const std::string& message,
             const std::string& file = "", int line = 0, const std::string& function = "");
    void log(const LogEntry& entry);
    
    // Convenience logging methods
    void trace(const std::string& message, const std::string& category = "");
    void debug(const std::string& message, const std::string& category = "");
    void info(const std::string& message, const std::string& category = "");
    void warning(const std::string& message, const std::string& category = "");
    void error(const std::string& message, const std::string& category = "");
    void critical(const std::string& message, const std::string& category = "");
    
    // Advanced logging with metadata
    void logWithMetadata(LogLevel level, const std::string& category, const std::string& message,
                        const std::unordered_map<std::string, std::string>& metadata,
                        const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance and monitoring
    PerformanceMetrics getMetrics() const;
    void resetMetrics();
    void setMaxQueueSize(size_t maxSize);
    void setAsyncMode(bool async);
    bool isAsyncMode() const;
    
    // Control methods
    void flush();
    void close();
    void startAsyncLogging();
    void stopAsyncLogging();
    
    // Utility methods
    bool shouldLog(LogLevel level, const std::string& category = "") const;
    std::string formatLogLevel(LogLevel level);
    static std::string getCurrentTimestamp();
    static std::string getThreadId();
    
    // Configuration from file
    bool loadConfiguration(const std::filesystem::path& configFile);
    bool saveConfiguration(const std::filesystem::path& configFile) const;
    
    // Signal handling for log rotation
    void handleSignal(int signal);
    
    ~LoggingManager();

private:
    LoggingManager();
    LoggingManager(const LoggingManager&) = delete;
    LoggingManager& operator=(const LoggingManager&) = delete;
    
    // Core members
    LogLevel globalLogLevel_;
    std::unordered_map<std::string, LogLevel> categoryLogLevels_;
    std::vector<std::unique_ptr<LogDestination>> destinations_;
    std::unique_ptr<LogFormatter> defaultFormatter_;
    std::unordered_map<std::string, std::unique_ptr<LogFormatter>> categoryFormatters_;
    
    // Async logging
    std::atomic<bool> asyncMode_;
    std::atomic<bool> running_;
    std::unique_ptr<std::thread> loggingThread_;
    std::queue<LogEntry> logQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    size_t maxQueueSize_;
    
    // Performance metrics
    mutable std::mutex metricsMutex_;
    PerformanceMetrics metrics_;
    std::chrono::steady_clock::time_point startTime_;
    
    // Internal methods
    void processLogEntry(const LogEntry& entry);
    void asyncLoggingLoop();
    LogFormatter* getFormatter(const std::string& category) const;
    void updateMetrics(LogLevel level, std::chrono::milliseconds duration);
    bool enqueueLogEntry(const LogEntry& entry);
};

// Scoped logger for RAII-style logging
class ScopedLogger {
public:
    ScopedLogger(const std::string& scope, LogLevel level = LogLevel::Info);
    ~ScopedLogger();
    
    void setMetadata(const std::string& key, const std::string& value);
    void log(LogLevel level, const std::string& message);

private:
    std::string scope_;
    LogLevel level_;
    std::chrono::steady_clock::time_point startTime_;
    std::unordered_map<std::string, std::string> metadata_;
};

// Performance timer for logging execution times
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name, LogLevel level = LogLevel::Debug);
    ~PerformanceTimer();
    
    void checkpoint(const std::string& checkpointName);
    void finish();

private:
    std::string name_;
    LogLevel level_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastCheckpoint_;
    bool finished_;
};

} // namespace utils

// Convenience macros for logging
#define LOG_TRACE(message) utils::LoggingManager::getInstance().trace(message, __FILE__ ":" + std::to_string(__LINE__))
#define LOG_DEBUG(message) utils::LoggingManager::getInstance().debug(message, __FILE__ ":" + std::to_string(__LINE__))
#define LOG_INFO(message) utils::LoggingManager::getInstance().info(message, __FILE__ ":" + std::to_string(__LINE__))
#define LOG_WARNING(message) utils::LoggingManager::getInstance().warning(message, __FILE__ ":" + std::to_string(__LINE__))
#define LOG_ERROR(message) utils::LoggingManager::getInstance().error(message, __FILE__ ":" + std::to_string(__LINE__))
#define LOG_CRITICAL(message) utils::LoggingManager::getInstance().critical(message, __FILE__ ":" + std::to_string(__LINE__))

#define LOG_CATEGORY(level, category, message) \
    utils::LoggingManager::getInstance().log(level, category, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_SCOPED(scope) utils::ScopedLogger __scoped_logger__(scope)
#define LOG_TIMER(name) utils::PerformanceTimer __perf_timer__(name)
