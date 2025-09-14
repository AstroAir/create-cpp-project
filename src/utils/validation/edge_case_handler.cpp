#include "../validation/edge_case_handler.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <mutex>
#include <thread>

#ifdef _WIN32
#include <windows.h>
// #include <psapi.h>  // Temporarily disabled due to compilation issues
#else
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <unistd.h>
#endif

namespace utils {

// CreateMissingDirectoryStrategy implementation
bool CreateMissingDirectoryStrategy::canRecover(const EdgeCaseInfo& info) const {
    return info.type == EdgeCaseType::MissingDirectory && !info.affectedPath.empty() &&
           info.severity != EdgeCaseSeverity::Critical;
}

bool CreateMissingDirectoryStrategy::attemptRecovery(const EdgeCaseInfo& info) {
    try {
        std::filesystem::create_directories(info.affectedPath);
        spdlog::info("Successfully created missing directory: {}", info.affectedPath.string());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to create directory {}: {} - Check parent directory permissions",
                      info.affectedPath.string(), e.what());
        return false;
    }
}

// CreateEmptyFileStrategy implementation
bool CreateEmptyFileStrategy::canRecover(const EdgeCaseInfo& info) const {
    return info.type == EdgeCaseType::EmptyFile && !info.affectedPath.empty() &&
           info.severity != EdgeCaseSeverity::Critical;
}

bool CreateEmptyFileStrategy::attemptRecovery(const EdgeCaseInfo& info) {
    try {
        // Ensure parent directory exists
        std::filesystem::create_directories(info.affectedPath.parent_path());

        std::ofstream file(info.affectedPath);
        if (file.is_open()) {
            file.close();
            spdlog::info("Successfully created empty file: {}", info.affectedPath.string());
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("Failed to create file {}: {} - Check directory permissions and disk space",
                      info.affectedPath.string(), e.what());
        return false;
    }
}

// RetryWithDelayStrategy implementation
RetryWithDelayStrategy::RetryWithDelayStrategy(std::chrono::milliseconds delay, int maxRetries)
    : delay_(delay), maxRetries_(maxRetries) {}

bool RetryWithDelayStrategy::canRecover(const EdgeCaseInfo& info) const {
    return info.type == EdgeCaseType::TemporaryFailure || info.type == EdgeCaseType::FileInUse ||
           info.type == EdgeCaseType::NetworkUnavailable;
}

bool RetryWithDelayStrategy::attemptRecovery(const EdgeCaseInfo& info) {
    std::string key =
            info.affectedPath.string() + "_" + std::to_string(static_cast<int>(info.type));

    if (retryCount_[key] >= maxRetries_) {
        spdlog::warn("Maximum retry attempts reached for: {}", info.description);
        return false;
    }

    retryCount_[key]++;
    spdlog::info("Retrying operation (attempt {}/{}): {}", retryCount_[key], maxRetries_,
                 info.description);

    std::this_thread::sleep_for(delay_);
    return true;  // Indicate that retry is possible, actual retry logic is handled by caller
}

// FallbackPathStrategy implementation
FallbackPathStrategy::FallbackPathStrategy(std::vector<std::filesystem::path> fallbackPaths)
    : fallbackPaths_(std::move(fallbackPaths)) {}

bool FallbackPathStrategy::canRecover(const EdgeCaseInfo& info) const {
    return !fallbackPaths_.empty() && (info.type == EdgeCaseType::MissingDirectory ||
                                       info.type == EdgeCaseType::InsufficientPermissions ||
                                       info.type == EdgeCaseType::PathTooLong);
}

bool FallbackPathStrategy::attemptRecovery(const EdgeCaseInfo& info) {
    for (const auto& fallbackPath : fallbackPaths_) {
        try {
            if (std::filesystem::exists(fallbackPath) ||
                std::filesystem::create_directories(fallbackPath)) {
                spdlog::info("Using fallback path: {} instead of {}", fallbackPath.string(),
                             info.affectedPath.string());
                return true;
            }
        } catch (const std::exception& e) {
            spdlog::debug("Fallback path {} failed: {}", fallbackPath.string(), e.what());
            continue;
        }
    }
    return false;
}

// PermissionFixStrategy implementation
bool PermissionFixStrategy::canRecover(const EdgeCaseInfo& info) const {
    return info.type == EdgeCaseType::InsufficientPermissions && !info.affectedPath.empty();
}

bool PermissionFixStrategy::attemptRecovery(const EdgeCaseInfo& info) {
    try {
#ifdef _WIN32
        // On Windows, try to set file attributes
        DWORD attributes = GetFileAttributesA(info.affectedPath.string().c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES) {
            attributes &= ~FILE_ATTRIBUTE_READONLY;
            if (SetFileAttributesA(info.affectedPath.string().c_str(), attributes)) {
                spdlog::info("Fixed file permissions for: {}", info.affectedPath.string());
                return true;
            }
        }
#else
        // On Unix-like systems, try to set permissions
        std::filesystem::permissions(info.affectedPath, std::filesystem::perms::owner_all,
                                     std::filesystem::perm_options::add);
        spdlog::info("Fixed file permissions for: {}", info.affectedPath.string());
        return true;
#endif
    } catch (const std::exception& e) {
        spdlog::error(
                "Failed to fix permissions for {}: {} - Run as administrator or check file "
                "ownership",
                info.affectedPath.string(), e.what());
        return false;
    }

    return false;
}

// EdgeCaseDetector implementation
std::optional<EdgeCaseInfo> EdgeCaseDetector::checkFileAccess(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        EdgeCaseInfo info(EdgeCaseType::EmptyFile, EdgeCaseSeverity::Warning,
                          "File does not exist");
        info.affectedPath = path;
        return info;
    }

    if (std::filesystem::is_empty(path)) {
        EdgeCaseInfo info(EdgeCaseType::EmptyFile, EdgeCaseSeverity::Info, "File is empty");
        info.affectedPath = path;
        return info;
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkDirectoryAccess(
        const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        EdgeCaseInfo info(EdgeCaseType::MissingDirectory, EdgeCaseSeverity::Error,
                          "Directory does not exist");
        info.affectedPath = path;
        return info;
    }

    if (!std::filesystem::is_directory(path)) {
        EdgeCaseInfo info(EdgeCaseType::InvalidCharacters, EdgeCaseSeverity::Error,
                          "Path exists but is not a directory");
        info.affectedPath = path;
        return info;
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkDiskSpace(const std::filesystem::path& path,
                                                             size_t requiredBytes) {
    try {
        auto spaceInfo = std::filesystem::space(path.parent_path());

        if (requiredBytes > 0 && spaceInfo.available < requiredBytes) {
            EdgeCaseInfo info(EdgeCaseType::DiskSpaceLow, EdgeCaseSeverity::Error,
                              "Insufficient disk space");
            info.affectedPath = path;
            info.metadata["required_bytes"] = std::to_string(requiredBytes);
            info.metadata["available_bytes"] = std::to_string(spaceInfo.available);
            return info;
        }

        // Warn if less than 100MB available
        if (spaceInfo.available < 100 * 1024 * 1024) {
            EdgeCaseInfo info(EdgeCaseType::DiskSpaceLow, EdgeCaseSeverity::Warning,
                              "Low disk space");
            info.affectedPath = path;
            info.metadata["available_bytes"] = std::to_string(spaceInfo.available);
            return info;
        }

    } catch (const std::exception& e) {
        EdgeCaseInfo info(EdgeCaseType::Unknown, EdgeCaseSeverity::Warning,
                          "Could not check disk space: " + std::string(e.what()));
        info.affectedPath = path;
        return info;
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkPathLength(const std::filesystem::path& path) {
    const size_t maxPathLength =
#ifdef _WIN32
            260;  // MAX_PATH on Windows
#else
            4096;  // PATH_MAX on most Unix systems
#endif

    if (path.string().length() > maxPathLength) {
        EdgeCaseInfo info(EdgeCaseType::PathTooLong, EdgeCaseSeverity::Error,
                          "Path exceeds maximum length");
        info.affectedPath = path;
        info.metadata["path_length"] = std::to_string(path.string().length());
        info.metadata["max_length"] = std::to_string(maxPathLength);
        return info;
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkInvalidCharacters(const std::string& filename) {
    const std::string invalidChars =
#ifdef _WIN32
            "<>:\"|?*";
#else
            "\0";  // Only null character is invalid on Unix
#endif

    for (char c : filename) {
        if (invalidChars.find(c) != std::string::npos || c < 32) {
            EdgeCaseInfo info(EdgeCaseType::InvalidCharacters, EdgeCaseSeverity::Error,
                              "Filename contains invalid characters");
            info.metadata["invalid_char"] = std::string(1, c);
            info.metadata["char_code"] = std::to_string(static_cast<int>(c));
            return info;
        }
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkMemoryAvailability(size_t requiredBytes) {
#ifdef _WIN32
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus)) {
        if (requiredBytes > 0 && memStatus.ullAvailPhys < requiredBytes) {
            EdgeCaseInfo info(EdgeCaseType::MemoryLow, EdgeCaseSeverity::Error,
                              "Insufficient memory available");
            info.metadata["required_bytes"] = std::to_string(requiredBytes);
            info.metadata["available_bytes"] = std::to_string(memStatus.ullAvailPhys);
            return info;
        }

        // Warn if less than 100MB available
        if (memStatus.ullAvailPhys < 100 * 1024 * 1024) {
            EdgeCaseInfo info(EdgeCaseType::MemoryLow, EdgeCaseSeverity::Warning,
                              "Low memory available");
            info.metadata["available_bytes"] = std::to_string(memStatus.ullAvailPhys);
            return info;
        }
    }
#else
    // Unix-like systems - simplified check
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long pageSize = sysconf(_SC_PAGE_SIZE);
    if (pages > 0 && pageSize > 0) {
        size_t availableMemory = pages * pageSize;

        if (requiredBytes > 0 && availableMemory < requiredBytes) {
            EdgeCaseInfo info(EdgeCaseType::MemoryLow, EdgeCaseSeverity::Error,
                              "Insufficient memory available");
            info.metadata["required_bytes"] = std::to_string(requiredBytes);
            info.metadata["available_bytes"] = std::to_string(availableMemory);
            return info;
        }

        if (availableMemory < 100 * 1024 * 1024) {
            EdgeCaseInfo info(EdgeCaseType::MemoryLow, EdgeCaseSeverity::Warning,
                              "Low memory available");
            info.metadata["available_bytes"] = std::to_string(availableMemory);
            return info;
        }
    }
#endif

    return std::nullopt;
}

std::vector<EdgeCaseInfo> EdgeCaseDetector::performComprehensiveCheck(
        const std::filesystem::path& path) {
    std::vector<EdgeCaseInfo> edgeCases;

    // Check file access
    auto fileCheck = checkFileAccess(path);
    if (fileCheck)
        edgeCases.push_back(*fileCheck);

    // Check path length
    auto pathCheck = checkPathLength(path);
    if (pathCheck)
        edgeCases.push_back(*pathCheck);

    // Check invalid characters
    auto charCheck = checkInvalidCharacters(path.filename().string());
    if (charCheck)
        edgeCases.push_back(*charCheck);

    // Check disk space
    auto diskCheck = checkDiskSpace(path);
    if (diskCheck)
        edgeCases.push_back(*diskCheck);

    // Check permissions
    auto readCheck = checkReadPermissions(path);
    if (readCheck)
        edgeCases.push_back(*readCheck);

    auto writeCheck = checkWritePermissions(path);
    if (writeCheck)
        edgeCases.push_back(*writeCheck);

    return edgeCases;
}

std::vector<EdgeCaseInfo> EdgeCaseDetector::performSystemCheck() {
    std::vector<EdgeCaseInfo> edgeCases;

    // Check memory
    auto memCheck = checkMemoryAvailability();
    if (memCheck)
        edgeCases.push_back(*memCheck);

    // Check process limits
    auto procCheck = checkProcessLimits();
    if (procCheck)
        edgeCases.push_back(*procCheck);

    return edgeCases;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkReadPermissions(
        const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }

    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            EdgeCaseInfo info(EdgeCaseType::InsufficientPermissions, EdgeCaseSeverity::Error,
                              "Cannot read file - insufficient permissions");
            info.affectedPath = path;
            return info;
        }
    } catch (const std::exception& e) {
        EdgeCaseInfo info(EdgeCaseType::InsufficientPermissions, EdgeCaseSeverity::Error,
                          "Read permission check failed: " + std::string(e.what()));
        info.affectedPath = path;
        return info;
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkWritePermissions(
        const std::filesystem::path& path) {
    try {
        if (std::filesystem::exists(path)) {
            // Try to open existing file for writing
            std::ofstream file(path, std::ios::app);
            if (!file.is_open()) {
                EdgeCaseInfo info(EdgeCaseType::InsufficientPermissions, EdgeCaseSeverity::Error,
                                  "Cannot write to file - insufficient permissions");
                info.affectedPath = path;
                return info;
            }
        } else {
            // Try to create new file
            std::ofstream file(path);
            if (!file.is_open()) {
                EdgeCaseInfo info(EdgeCaseType::InsufficientPermissions, EdgeCaseSeverity::Error,
                                  "Cannot create file - insufficient permissions");
                info.affectedPath = path;
                return info;
            }
            file.close();
            std::filesystem::remove(path);  // Clean up test file
        }
    } catch (const std::exception& e) {
        EdgeCaseInfo info(EdgeCaseType::InsufficientPermissions, EdgeCaseSeverity::Error,
                          "Write permission check failed: " + std::string(e.what()));
        info.affectedPath = path;
        return info;
    }

    return std::nullopt;
}

std::optional<EdgeCaseInfo> EdgeCaseDetector::checkProcessLimits() {
    // This is a simplified check - in a real implementation, you'd check actual process limits
    EdgeCaseInfo info(EdgeCaseType::ProcessLimit, EdgeCaseSeverity::Info,
                      "Process limit check not implemented");
    return std::nullopt;
}

// EdgeCaseHandler implementation
EdgeCaseHandler& EdgeCaseHandler::getInstance() {
    static EdgeCaseHandler instance;
    return instance;
}

EdgeCaseHandler::EdgeCaseHandler() {
    initializeDefaultStrategies();
}

void EdgeCaseHandler::initializeDefaultStrategies() {
    registerStrategy(std::make_unique<CreateMissingDirectoryStrategy>());
    registerStrategy(std::make_unique<CreateEmptyFileStrategy>());
    registerStrategy(std::make_unique<RetryWithDelayStrategy>());
    registerStrategy(std::make_unique<PermissionFixStrategy>());

    // Add fallback paths for common scenarios
    std::vector<std::filesystem::path> fallbackPaths = {std::filesystem::temp_directory_path(),
                                                        std::filesystem::current_path() / "temp",
                                                        std::filesystem::current_path() / "backup"};
    registerStrategy(std::make_unique<FallbackPathStrategy>(fallbackPaths));
}

void EdgeCaseHandler::registerStrategy(std::unique_ptr<EdgeCaseRecoveryStrategy> strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    strategies_.push_back(std::move(strategy));

    // Sort strategies by priority (highest first)
    std::sort(strategies_.begin(), strategies_.end(),
              [](const auto& a, const auto& b) { return a->getPriority() > b->getPriority(); });
}

void EdgeCaseHandler::clearStrategies() {
    std::lock_guard<std::mutex> lock(mutex_);
    strategies_.clear();
}

std::vector<EdgeCaseRecoveryStrategy*> EdgeCaseHandler::getStrategies(EdgeCaseType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<EdgeCaseRecoveryStrategy*> result;

    for (const auto& strategy : strategies_) {
        // For now, return all strategies. In a more sophisticated implementation,
        // you could filter by edge case type
        result.push_back(strategy.get());
    }

    return result;
}

std::string EdgeCaseHandler::edgeCaseTypeToString(EdgeCaseType type) const {
    switch (type) {
        case EdgeCaseType::EmptyFile:
            return "EmptyFile";
        case EdgeCaseType::MissingDirectory:
            return "MissingDirectory";
        case EdgeCaseType::InsufficientPermissions:
            return "InsufficientPermissions";
        case EdgeCaseType::DiskSpaceLow:
            return "DiskSpaceLow";
        case EdgeCaseType::FileInUse:
            return "FileInUse";
        case EdgeCaseType::InvalidCharacters:
            return "InvalidCharacters";
        case EdgeCaseType::PathTooLong:
            return "PathTooLong";
        case EdgeCaseType::NetworkUnavailable:
            return "NetworkUnavailable";
        case EdgeCaseType::MemoryLow:
            return "MemoryLow";
        case EdgeCaseType::ProcessLimit:
            return "ProcessLimit";
        case EdgeCaseType::TemporaryFailure:
            return "TemporaryFailure";
        default:
            return "Unknown";
    }
}

std::string EdgeCaseHandler::edgeCaseSeverityToString(EdgeCaseSeverity severity) const {
    switch (severity) {
        case EdgeCaseSeverity::Info:
            return "Info";
        case EdgeCaseSeverity::Warning:
            return "Warning";
        case EdgeCaseSeverity::Error:
            return "Error";
        case EdgeCaseSeverity::Critical:
            return "Critical";
        default:
            return "Unknown";
    }
}

}  // namespace utils
