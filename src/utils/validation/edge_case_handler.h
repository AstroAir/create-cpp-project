#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace utils {

// Edge case categories
enum class EdgeCaseType {
    EmptyFile,
    MissingDirectory,
    InsufficientPermissions,
    DiskSpaceLow,
    FileInUse,
    InvalidCharacters,
    PathTooLong,
    NetworkUnavailable,
    MemoryLow,
    ProcessLimit,
    TemporaryFailure,
    Unknown
};

// Edge case severity levels
enum class EdgeCaseSeverity {
    Info,     // Informational, can continue
    Warning,  // Warning, might affect functionality
    Error,    // Error, operation failed but recoverable
    Critical  // Critical, operation failed and not recoverable
};

// Edge case information
struct EdgeCaseInfo {
    EdgeCaseType type;
    EdgeCaseSeverity severity;
    std::string description;
    std::string context;
    std::filesystem::path affectedPath;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;

    EdgeCaseInfo(EdgeCaseType type, EdgeCaseSeverity severity, const std::string& desc)
        : type(type),
          severity(severity),
          description(desc),
          timestamp(std::chrono::system_clock::now()) {}
};

// Edge case recovery strategy interface
class EdgeCaseRecoveryStrategy {
   public:
    virtual ~EdgeCaseRecoveryStrategy() = default;
    virtual bool canRecover(const EdgeCaseInfo& info) const = 0;
    virtual bool attemptRecovery(const EdgeCaseInfo& info) = 0;
    virtual std::string getDescription() const = 0;
    virtual int getPriority() const = 0;  // Higher priority strategies are tried first
};

// Specific recovery strategies
class CreateMissingDirectoryStrategy : public EdgeCaseRecoveryStrategy {
   public:
    bool canRecover(const EdgeCaseInfo& info) const override;
    bool attemptRecovery(const EdgeCaseInfo& info) override;
    std::string getDescription() const override {
        return "Create missing directory";
    }
    int getPriority() const override {
        return 100;
    }
};

class CreateEmptyFileStrategy : public EdgeCaseRecoveryStrategy {
   public:
    bool canRecover(const EdgeCaseInfo& info) const override;
    bool attemptRecovery(const EdgeCaseInfo& info) override;
    std::string getDescription() const override {
        return "Create empty file";
    }
    int getPriority() const override {
        return 90;
    }
};

class RetryWithDelayStrategy : public EdgeCaseRecoveryStrategy {
   public:
    explicit RetryWithDelayStrategy(
            std::chrono::milliseconds delay = std::chrono::milliseconds(1000), int maxRetries = 3);
    bool canRecover(const EdgeCaseInfo& info) const override;
    bool attemptRecovery(const EdgeCaseInfo& info) override;
    std::string getDescription() const override {
        return "Retry with delay";
    }
    int getPriority() const override {
        return 50;
    }

   private:
    std::chrono::milliseconds delay_;
    int maxRetries_;
    mutable std::unordered_map<std::string, int> retryCount_;
};

class FallbackPathStrategy : public EdgeCaseRecoveryStrategy {
   public:
    explicit FallbackPathStrategy(std::vector<std::filesystem::path> fallbackPaths);
    bool canRecover(const EdgeCaseInfo& info) const override;
    bool attemptRecovery(const EdgeCaseInfo& info) override;
    std::string getDescription() const override {
        return "Use fallback path";
    }
    int getPriority() const override {
        return 80;
    }

   private:
    std::vector<std::filesystem::path> fallbackPaths_;
};

class PermissionFixStrategy : public EdgeCaseRecoveryStrategy {
   public:
    bool canRecover(const EdgeCaseInfo& info) const override;
    bool attemptRecovery(const EdgeCaseInfo& info) override;
    std::string getDescription() const override {
        return "Fix file permissions";
    }
    int getPriority() const override {
        return 70;
    }
};

// Edge case detection functions
class EdgeCaseDetector {
   public:
    // File system edge cases
    static std::optional<EdgeCaseInfo> checkFileAccess(const std::filesystem::path& path);
    static std::optional<EdgeCaseInfo> checkDirectoryAccess(const std::filesystem::path& path);
    static std::optional<EdgeCaseInfo> checkDiskSpace(const std::filesystem::path& path,
                                                      size_t requiredBytes = 0);
    static std::optional<EdgeCaseInfo> checkPathLength(const std::filesystem::path& path);
    static std::optional<EdgeCaseInfo> checkInvalidCharacters(const std::string& filename);

    // System resource edge cases
    static std::optional<EdgeCaseInfo> checkMemoryAvailability(size_t requiredBytes = 0);
    static std::optional<EdgeCaseInfo> checkProcessLimits();
    static std::optional<EdgeCaseInfo> checkNetworkConnectivity(const std::string& host = "");

    // Content edge cases
    static std::optional<EdgeCaseInfo> checkFileContent(const std::filesystem::path& path);
    static std::optional<EdgeCaseInfo> checkFileSize(const std::filesystem::path& path,
                                                     size_t minSize = 0, size_t maxSize = SIZE_MAX);

    // Permission edge cases
    static std::optional<EdgeCaseInfo> checkReadPermissions(const std::filesystem::path& path);
    static std::optional<EdgeCaseInfo> checkWritePermissions(const std::filesystem::path& path);
    static std::optional<EdgeCaseInfo> checkExecutePermissions(const std::filesystem::path& path);

    // Comprehensive checks
    static std::vector<EdgeCaseInfo> performComprehensiveCheck(const std::filesystem::path& path);
    static std::vector<EdgeCaseInfo> performSystemCheck();
};

// Main edge case handler
class EdgeCaseHandler {
   public:
    static EdgeCaseHandler& getInstance();

    // Strategy management
    void registerStrategy(std::unique_ptr<EdgeCaseRecoveryStrategy> strategy);
    void clearStrategies();
    std::vector<EdgeCaseRecoveryStrategy*> getStrategies(EdgeCaseType type) const;

    // Edge case handling
    bool handleEdgeCase(const EdgeCaseInfo& info);
    std::vector<EdgeCaseInfo> handleMultipleEdgeCases(const std::vector<EdgeCaseInfo>& cases);

    // Automatic detection and handling
    bool checkAndHandleFileOperation(const std::filesystem::path& path,
                                     const std::string& operation = "access");
    bool checkAndHandleDirectoryOperation(const std::filesystem::path& path,
                                          const std::string& operation = "access");
    bool checkAndHandleSystemResources();

    // Configuration
    void setAutoRecovery(bool enabled) {
        autoRecovery_ = enabled;
    }
    void setMaxRecoveryAttempts(int maxAttempts) {
        maxRecoveryAttempts_ = maxAttempts;
    }
    void setRecoveryTimeout(std::chrono::milliseconds timeout) {
        recoveryTimeout_ = timeout;
    }

    // Reporting and logging
    void logEdgeCase(const EdgeCaseInfo& info) const;
    void generateEdgeCaseReport() const;
    std::vector<EdgeCaseInfo> getRecentEdgeCases(
            std::chrono::minutes timeWindow = std::chrono::minutes(60)) const;

    // Statistics
    struct EdgeCaseStats {
        size_t totalCases = 0;
        size_t resolvedCases = 0;
        size_t unresolvedCases = 0;
        std::unordered_map<EdgeCaseType, size_t> casesByType;
        std::unordered_map<EdgeCaseSeverity, size_t> casesBySeverity;
        double resolutionRate = 0.0;
    };

    EdgeCaseStats getStatistics() const;
    void resetStatistics();

   private:
    EdgeCaseHandler();

    std::vector<std::unique_ptr<EdgeCaseRecoveryStrategy>> strategies_;
    std::vector<EdgeCaseInfo> edgeCaseHistory_;
    bool autoRecovery_ = true;
    int maxRecoveryAttempts_ = 3;
    std::chrono::milliseconds recoveryTimeout_{5000};

    mutable std::mutex mutex_;

    void initializeDefaultStrategies();
    bool attemptRecovery(const EdgeCaseInfo& info);
    void recordEdgeCase(const EdgeCaseInfo& info);
    std::string edgeCaseTypeToString(EdgeCaseType type) const;
    std::string edgeCaseSeverityToString(EdgeCaseSeverity severity) const;
};

// Utility functions for common edge case patterns
namespace EdgeCaseUtils {
// File operation wrappers with edge case handling
bool safeFileRead(const std::filesystem::path& path, std::string& content);
bool safeFileWrite(const std::filesystem::path& path, const std::string& content);
bool safeFileDelete(const std::filesystem::path& path);
bool safeDirectoryCreate(const std::filesystem::path& path);
bool safeDirectoryDelete(const std::filesystem::path& path);

// Path validation and sanitization
std::filesystem::path sanitizePath(const std::filesystem::path& path);
bool isValidPath(const std::filesystem::path& path);
std::filesystem::path generateAlternativePath(const std::filesystem::path& path);

// System resource checks
bool hasEnoughDiskSpace(const std::filesystem::path& path, size_t requiredBytes);
bool hasEnoughMemory(size_t requiredBytes);
bool isSystemResourcesHealthy();

// Recovery helpers
bool waitForFileAvailability(const std::filesystem::path& path,
                             std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));
bool createParentDirectories(const std::filesystem::path& path);
bool fixFilePermissions(const std::filesystem::path& path);

// Fallback mechanisms
std::filesystem::path getTempFallbackPath(const std::filesystem::path& originalPath);
std::string getDefaultContent(const std::filesystem::path& path);
std::vector<std::filesystem::path> generateFallbackPaths(const std::filesystem::path& path);
}  // namespace EdgeCaseUtils

// RAII helper for edge case context
class EdgeCaseContext {
   public:
    explicit EdgeCaseContext(const std::string& operation, const std::filesystem::path& path = "");
    ~EdgeCaseContext();

    void addMetadata(const std::string& key, const std::string& value);
    void reportEdgeCase(EdgeCaseType type, EdgeCaseSeverity severity,
                        const std::string& description);

   private:
    std::string operation_;
    std::filesystem::path path_;
    std::unordered_map<std::string, std::string> metadata_;
    std::chrono::system_clock::time_point startTime_;
};

// Macros for easier edge case handling
#define EDGE_CASE_CONTEXT(op, path) EdgeCaseContext _edge_context(op, path)
#define REPORT_EDGE_CASE(type, severity, desc) _edge_context.reportEdgeCase(type, severity, desc)
#define CHECK_AND_HANDLE_FILE(path, op) \
    EdgeCaseHandler::getInstance().checkAndHandleFileOperation(path, op)
#define CHECK_AND_HANDLE_DIR(path, op) \
    EdgeCaseHandler::getInstance().checkAndHandleDirectoryOperation(path, op)

}  // namespace utils
