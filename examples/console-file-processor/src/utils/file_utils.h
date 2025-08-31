/**
 * @file file_utils.h
 * @brief File system utilities
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace utils {

/**
 * @brief File system utilities
 */
class FileUtils {
public:
    /**
     * @brief Read entire file content
     * @param filepath Path to file
     * @return File content as string
     */
    static std::string read_file(const std::filesystem::path& filepath);
    
    /**
     * @brief Write content to file
     * @param filepath Path to file
     * @param content Content to write
     * @return True if successful
     */
    static bool write_file(const std::filesystem::path& filepath, const std::string& content);
    
    /**
     * @brief Check if file exists
     * @param filepath Path to file
     * @return True if file exists
     */
    static bool file_exists(const std::filesystem::path& filepath);
    
    /**
     * @brief Get file extension
     * @param filepath Path to file
     * @return File extension
     */
    static std::string get_extension(const std::filesystem::path& filepath);
    
    /**
     * @brief Create directory if it doesn't exist
     * @param dirpath Directory path
     * @return True if successful
     */
    static bool create_directory(const std::filesystem::path& dirpath);
    
    /**
     * @brief List files in directory
     * @param dirpath Directory path
     * @param recursive Whether to search recursively
     * @return Vector of file paths
     */
    static std::vector<std::filesystem::path> list_files(
        const std::filesystem::path& dirpath, 
        bool recursive = false);
};

} // namespace utils
