/**
 * @file file_utils.cpp
 * @brief Implementation of file system utilities
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include "../core/file_utils.h"
#include <fstream>
#include <sstream>

namespace utils {

std::string FileUtils::read_file(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath.string());
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool FileUtils::write_file(const std::filesystem::path& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool FileUtils::file_exists(const std::filesystem::path& filepath) {
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath);
}

std::string FileUtils::get_extension(const std::filesystem::path& filepath) {
    return filepath.extension().string();
}

bool FileUtils::create_directory(const std::filesystem::path& dirpath) {
    try {
        return std::filesystem::create_directories(dirpath);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

std::vector<std::filesystem::path> FileUtils::list_files(
    const std::filesystem::path& dirpath, 
    bool recursive) {
    
    std::vector<std::filesystem::path> files;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirpath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dirpath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Return empty vector on error
    }
    
    return files;
}

} // namespace utils
