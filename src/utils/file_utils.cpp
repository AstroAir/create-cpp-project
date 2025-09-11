#include "file_utils.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <stdexcept>
#include <system_error>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace utils {
bool FileUtils::createDirectory(const std::string& path) {
    try {
        // Check if path is empty
        if (path.empty()) {
            spdlog::error("Directory path cannot be empty. Provide a valid directory path");
            return false;
        }

        // Check if path already exists
        if (fs::exists(path)) {
            if (fs::is_directory(path)) {
                spdlog::debug("Directory already exists: {}", path);
                return true;
            } else {
                spdlog::error(
                        "Cannot create directory '{}': Path exists but is not a directory. Choose "
                        "a different path or remove the existing file",
                        path);
                return false;
            }
        }

        // Check parent directory permissions
        fs::path parentPath = fs::path(path).parent_path();
        if (!parentPath.empty() && fs::exists(parentPath)) {
            std::error_code ec;
            auto perms = fs::status(parentPath, ec).permissions();
            if (ec) {
                spdlog::error(
                        "Cannot check permissions for '{}': {}. Verify parent directory exists and "
                        "is accessible",
                        parentPath.string(), ec.message());
                return false;
            }

            // Check if we have write permissions (simplified check)
            if ((perms & fs::perms::owner_write) == fs::perms::none &&
                (perms & fs::perms::group_write) == fs::perms::none &&
                (perms & fs::perms::others_write) == fs::perms::none) {
                spdlog::error(
                        "Permission denied for creating directory in '{}'. Run with appropriate "
                        "permissions or choose a different location",
                        parentPath.string());
                return false;
            }
        }

        // Attempt to create the directory
        std::error_code ec;
        bool result = fs::create_directories(path, ec);

        if (ec) {
            std::string suggestion;
            if (ec == std::errc::permission_denied) {
                suggestion =
                        "Run with administrator/root privileges or choose a different location";
            } else if (ec == std::errc::no_space_on_device) {
                suggestion = "Free up disk space or choose a different drive";
            } else if (ec == std::errc::filename_too_long) {
                suggestion = "Use a shorter path name";
            } else {
                suggestion = "Check path validity and system permissions";
            }

            spdlog::error("Failed to create directory '{}': {}. {}", path, ec.message(),
                          suggestion);
            return false;
        }

        spdlog::info("Successfully created directory: {}", path);
        return result;

    } catch (const std::exception& e) {
        spdlog::error(
                "Failed to create directory '{}': {}. Check path validity and system permissions",
                path, e.what());
        return false;
    }
}

bool FileUtils::writeToFile(const std::string& path, const std::string& content) {
    try {
        // Validate input parameters
        if (path.empty()) {
            spdlog::error("File path cannot be empty. Provide a valid file path");
            return false;
        }

        // Check if content is too large (e.g., > 1GB)
        const size_t MAX_FILE_SIZE = 1024 * 1024 * 1024;  // 1GB
        if (content.size() > MAX_FILE_SIZE) {
            spdlog::error(
                    "File content too large ({} bytes). Split content into smaller files or use "
                    "streaming",
                    content.size());
            return false;
        }

        // Create parent directories if they don't exist
        fs::path filePath(path);
        fs::path parentPath = filePath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            std::error_code ec;
            fs::create_directories(parentPath, ec);
            if (ec) {
                spdlog::error(
                        "Failed to create parent directories for '{}': {}. Check permissions and "
                        "disk space",
                        parentPath.string(), ec.message());
                return false;
            }
        }

        // Check if file exists and handle backup if needed
        bool fileExists = fs::exists(path);
        if (fileExists) {
            // Check if file is read-only
            std::error_code ec;
            auto perms = fs::status(path, ec).permissions();
            if (!ec && (perms & fs::perms::owner_write) == fs::perms::none) {
                spdlog::error(
                        "Cannot write to file '{}': File is read-only. Change permissions or "
                        "choose a different file",
                        path);
                return false;
            }
        }

        // Attempt to open and write to file
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            std::string suggestion;
            if (!fileExists) {
                suggestion = "Check if parent directory exists and you have write permissions";
            } else {
                suggestion =
                        "Check if file is locked by another process or you have write permissions";
            }
            spdlog::error("Could not open file '{}' for writing. {}", path, suggestion);
            return false;
        }

        // Write content and check for errors
        file << content;
        if (file.fail()) {
            spdlog::error(
                    "Write operation failed for file '{}'. Check disk space and file permissions",
                    path);
            return false;
        }

        // Explicitly close and check for errors
        file.close();
        if (file.fail()) {
            spdlog::error(
                    "Failed to close file '{}' properly. Check disk space and file system "
                    "integrity",
                    path);
            return false;
        }

        spdlog::info("Successfully wrote {} bytes to file: {}", content.size(), path);
        return true;

    } catch (const std::exception& e) {
        spdlog::error(
                "Failed to write to file '{}': {}. Check file path, permissions, and disk space",
                path, e.what());
        return false;
    }
}

std::string FileUtils::readFromFile(const std::string& path) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);
        }
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    } catch (const std::exception& e) {
        spdlog::error("Error reading from file: {}", e.what());
        return "";
    }
}

bool FileUtils::fileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool FileUtils::directoryExists(const std::string& path) {
    return fs::exists(path) && fs::is_directory(path);
}

std::vector<std::string> FileUtils::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("Error listing files: {}", e.what());
    }
    return files;
}

bool FileUtils::copyFile(const std::string& source, const std::string& destination) {
    try {
        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error copying file: {}", e.what());
        return false;
    }
}

bool FileUtils::copyDirectory(const std::string& source, const std::string& destination) {
    try {
        if (!directoryExists(destination)) {
            createDirectory(destination);
        }

        for (const auto& entry : fs::directory_iterator(source)) {
            const auto& path = entry.path();
            const auto& targetPath = fs::path(destination) / path.filename();

            if (fs::is_directory(path)) {
                if (!copyDirectory(path.string(), targetPath.string())) {
                    return false;
                }
            } else {
                if (!copyFile(path.string(), targetPath.string())) {
                    return false;
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error copying directory: {}", e.what());
        return false;
    }
}

std::string FileUtils::getFileName(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string FileUtils::getDirectoryName(const std::string& path) {
    auto parent = fs::path(path).parent_path();
    if (parent.empty()) {
        return ".";
    }
    return parent.string();
}

std::string FileUtils::getFileExtension(const std::string& path) {
    return fs::path(path).extension().string();
}

std::string FileUtils::combinePath(const std::string& path1, const std::string& path2) {
    return (fs::path(path1) / fs::path(path2)).string();
}

bool FileUtils::setExecutable(const std::string& filePath) {
#ifdef _WIN32
    // Windows不需要设置执行权限
    (void)filePath;  // Suppress unused parameter warning
    return true;
#else
    // Unix系统使用chmod
    std::string command = "chmod +x \"" + filePath + "\"";
    return system(command.c_str()) == 0;
#endif
}

bool FileUtils::commandExists(const std::string& command) {
#ifdef _WIN32
    std::string cmd = "where " + command + " >nul 2>&1";
#else
    std::string cmd = "command -v " + command + " >/dev/null 2>&1";
#endif
    return system(cmd.c_str()) == 0;
}

// Enhanced methods removed to simplify build
}  // namespace utils
