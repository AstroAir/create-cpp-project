/**
 * @file file_analyzer.cpp
 * @brief Implementation of file analysis utilities
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include "file_analyzer.h"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace core {

std::string FileAnalyzer::detect_encoding(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "unknown";
    }
    
    // Check for BOM
    char bom[3];
    file.read(bom, 3);
    
    if (bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF') {
        return "UTF-8";
    } else if (bom[0] == '\xFF' && bom[1] == '\xFE') {
        return "UTF-16LE";
    } else if (bom[0] == '\xFE' && bom[1] == '\xFF') {
        return "UTF-16BE";
    }
    
    // Simple heuristic for ASCII vs UTF-8
    file.seekg(0);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    bool has_non_ascii = std::any_of(content.begin(), content.end(),
        [](unsigned char c) { return c > 127; });
    
    return has_non_ascii ? "UTF-8" : "ASCII";
}

std::string FileAnalyzer::detect_file_type(const std::filesystem::path& filepath) {
    auto extension = filepath.extension().string();
    
    if (extension.empty()) {
        return "text";
    }
    
    // Convert to lowercase
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".cpp" || extension == ".cc" || extension == ".cxx") {
        return "C++";
    } else if (extension == ".h" || extension == ".hpp" || extension == ".hxx") {
        return "C++ Header";
    } else if (extension == ".c") {
        return "C";
    } else if (extension == ".py") {
        return "Python";
    } else if (extension == ".js") {
        return "JavaScript";
    } else if (extension == ".txt") {
        return "Text";
    } else if (extension == ".md") {
        return "Markdown";
    } else if (extension == ".json") {
        return "JSON";
    } else if (extension == ".xml") {
        return "XML";
    } else if (extension == ".html" || extension == ".htm") {
        return "HTML";
    }
    
    return "Unknown";
}

bool FileAnalyzer::is_binary_file(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first 512 bytes and check for null bytes
    char buffer[512];
    file.read(buffer, sizeof(buffer));
    std::streamsize bytes_read = file.gcount();
    
    for (std::streamsize i = 0; i < bytes_read; ++i) {
        if (buffer[i] == '\0') {
            return true;
        }
    }
    
    return false;
}

std::size_t FileAnalyzer::get_file_size(const std::filesystem::path& filepath) {
    try {
        return std::filesystem::file_size(filepath);
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

} // namespace core
