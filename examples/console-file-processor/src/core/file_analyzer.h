/**
 * @file file_analyzer.h
 * @brief File analysis utilities
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <filesystem>

namespace core {

/**
 * @brief File analysis utilities
 */
class FileAnalyzer {
public:
    /**
     * @brief Analyze file encoding
     * @param filepath Path to file
     * @return Detected encoding
     */
    static std::string detect_encoding(const std::filesystem::path& filepath);
    
    /**
     * @brief Detect file type
     * @param filepath Path to file
     * @return Detected file type
     */
    static std::string detect_file_type(const std::filesystem::path& filepath);
    
    /**
     * @brief Check if file is binary
     * @param filepath Path to file
     * @return True if binary file
     */
    static bool is_binary_file(const std::filesystem::path& filepath);
    
    /**
     * @brief Get file size
     * @param filepath Path to file
     * @return File size in bytes
     */
    static std::size_t get_file_size(const std::filesystem::path& filepath);
};

} // namespace core
