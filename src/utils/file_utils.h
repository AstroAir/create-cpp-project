#pragma once
#include <chrono>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

namespace utils {

// Result structure for operations that can fail
template <typename T>
struct Result {
    T value;
    std::error_code error;
    std::string message;

    bool hasError() const {
        return static_cast<bool>(error);
    }
    operator bool() const {
        return !hasError();
    }
};

// File operation options
struct FileOptions {
    bool createParentDirectories = true;
    bool overwriteExisting = true;
    bool backupExisting = false;
    std::filesystem::perms permissions = std::filesystem::perms::none;
    bool followSymlinks = true;
};

// Directory listing options
struct ListOptions {
    bool recursive = false;
    bool includeDirectories = false;
    bool includeHidden = false;
    std::vector<std::string> extensions;  // Filter by extensions
    std::function<bool(const std::filesystem::path&)> filter;
};

class FileUtils {
   public:
    // Directory operations

    // Original API for backwards compatibility
    static bool createDirectory(const std::string& path);
    static Result<bool> createDirectoryStructure(const std::vector<std::filesystem::path>& paths,
                                                 const std::filesystem::path& basePath = ".");
    static Result<bool> removeDirectory(const std::filesystem::path& path, bool recursive = false);
    static bool directoryExists(const std::string& path);
    static Result<bool> isDirectoryEmpty(const std::filesystem::path& path);
    static Result<size_t> getDirectorySize(const std::filesystem::path& path,
                                           bool recursive = true);

    // File operations

    // Original API for backwards compatibility
    static bool writeToFile(const std::string& path, const std::string& content);
    static std::string readFromFile(const std::string& path);
    static std::vector<std::string> readLines(const std::string& path);
    static bool appendToFile(const std::string& path, const std::string& content);

    static bool fileExists(const std::string& path);
    static Result<bool> removeFile(const std::filesystem::path& path);
    static Result<std::uintmax_t> getFileSize(const std::filesystem::path& path);
    static Result<std::filesystem::file_time_type> getLastWriteTime(
            const std::filesystem::path& path);

    // Enhanced copying operations
    static Result<bool> copyFileEx(const std::filesystem::path& source,
                                   const std::filesystem::path& destination,
                                   const FileOptions& options = {});
    static Result<bool> copyDirectoryEx(const std::filesystem::path& source,
                                        const std::filesystem::path& destination,
                                        const FileOptions& options = {});

    // Original API for backwards compatibility
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool copyDirectory(const std::string& source, const std::string& destination);
    static Result<bool> moveFile(const std::filesystem::path& source,
                                 const std::filesystem::path& destination);

    // Enhanced listing operations
    static Result<std::vector<std::filesystem::path>> listFilesEx(
            const std::filesystem::path& directory, const ListOptions& options = {});

    // Original API for backwards compatibility
    static std::vector<std::string> listFiles(const std::string& directory);
    static Result<std::vector<std::filesystem::path>> listDirectories(
            const std::filesystem::path& directory);

    // Path manipulation with validation
    static std::string getFileName(const std::string& path);
    static std::string getFileNameWithoutExtension(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::filesystem::path getParentPath(const std::filesystem::path& path);
    static std::filesystem::path getAbsolutePath(const std::filesystem::path& path);
    static Result<std::filesystem::path> getRelativePath(const std::filesystem::path& from,
                                                         const std::filesystem::path& to);
    // Enhanced path combination with filesystem::path
    static std::filesystem::path combinePathEx(const std::filesystem::path& path1,
                                               const std::filesystem::path& path2);

    // Original API for backwards compatibility
    static std::string combinePath(const std::string& path1, const std::string& path2);

    // File permissions and attributes
    static Result<bool> setExecutableEx(const std::filesystem::path& filePath);

    // Original API for backwards compatibility
    static bool setExecutable(const std::string& filePath);
    static Result<bool> setPermissions(const std::filesystem::path& filePath,
                                       std::filesystem::perms permissions);
    static Result<std::filesystem::perms> getPermissions(const std::filesystem::path& filePath);
    static bool isExecutable(const std::filesystem::path& filePath);
    static bool isReadable(const std::filesystem::path& filePath);
    static bool isWritable(const std::filesystem::path& filePath);

    // System path utilities
    static std::filesystem::path getTempDirectory();
    static std::filesystem::path getCurrentDirectory();
    static Result<bool> setCurrentDirectory(const std::filesystem::path& path);
    static std::filesystem::path getHomeDirectory();
    static std::filesystem::path getExecutablePath();
    static std::filesystem::path getConfigDirectory();

    // Content manipulation
    static Result<bool> replaceInFile(const std::filesystem::path& filePath,
                                      const std::string& search, const std::string& replace);
    static Result<bool> writeLines(const std::filesystem::path& filePath,
                                   const std::vector<std::string>& lines);
    static Result<size_t> countLines(const std::filesystem::path& filePath);

    // Validation functions
    static bool isValidPath(const std::string& path);
    static bool isValidFileName(const std::string& fileName);
    static Result<bool> hasReadPermission(const std::filesystem::path& path);
    static Result<bool> hasWritePermission(const std::filesystem::path& path);
    static bool isPathSafe(const std::filesystem::path& path, const std::filesystem::path& baseDir);

    // Utility functions
    static bool commandExists(const std::string& command);
    static Result<std::string> findInPath(const std::string& command);
    static Result<bool> createBackup(const std::filesystem::path& filePath);
    static Result<std::vector<std::filesystem::path>> findFiles(
            const std::filesystem::path& directory, const std::string& pattern);

    // File monitoring and watching (basic implementation)
    static std::chrono::system_clock::time_point getFileModificationTime(
            const std::filesystem::path& path);
    static bool isFileNewer(const std::filesystem::path& file1, const std::filesystem::path& file2);
};
}  // namespace utils
