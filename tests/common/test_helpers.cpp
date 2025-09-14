#include "test_helpers.h"
#include <fstream>
#include <algorithm>
#include <sstream>

namespace test_utils {

// File system utilities implementation
namespace fs_utils {

bool containsAllFiles(const std::filesystem::path& dir, const std::vector<std::string>& expectedFiles) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return false;
    }

    for (const auto& expectedFile : expectedFiles) {
        std::filesystem::path filePath = dir / expectedFile;
        if (!std::filesystem::exists(filePath) || std::filesystem::is_directory(filePath)) {
            return false;
        }
    }

    return true;
}

bool containsAllDirs(const std::filesystem::path& dir, const std::vector<std::string>& expectedDirs) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return false;
    }

    for (const auto& expectedDir : expectedDirs) {
        std::filesystem::path dirPath = dir / expectedDir;
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
            return false;
        }
    }

    return true;
}

std::vector<std::filesystem::path> getFilesWithExtension(const std::filesystem::path& dir, const std::string& extension) {
    std::vector<std::filesystem::path> files;

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return files;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            files.push_back(entry.path());
        }
    }

    return files;
}

size_t countFiles(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return 0;
    }

    size_t count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            ++count;
        }
    }

    return count;
}

size_t countDirectories(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return 0;
    }

    size_t count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_directory()) {
            ++count;
        }
    }

    return count;
}

} // namespace fs_utils

// String utilities implementation
namespace string_utils {

bool containsAll(const std::string& text, const std::vector<std::string>& expectedSubstrings) {
    for (const auto& substring : expectedSubstrings) {
        if (text.find(substring) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool matchesPattern(const std::string& text, const std::string& pattern) {
    try {
        std::regex regex(pattern);
        return std::regex_search(text, regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::vector<std::string> split(const std::string& text, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = text.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(text.substr(start, end - start));
        start = end + delimiter.length();
        end = text.find(delimiter, start);
    }

    result.push_back(text.substr(start));
    return result;
}

std::string trim(const std::string& text) {
    const std::string whitespace = " \t\n\r\f\v";

    size_t start = text.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }

    size_t end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

std::string normalizeLineEndings(const std::string& text) {
    std::string result = text;

    // Replace \r\n with \n
    size_t pos = 0;
    while ((pos = result.find("\r\n", pos)) != std::string::npos) {
        result.replace(pos, 2, "\n");
        pos += 1;
    }

    // Replace remaining \r with \n
    pos = 0;
    while ((pos = result.find("\r", pos)) != std::string::npos) {
        result.replace(pos, 1, "\n");
        pos += 1;
    }

    return result;
}

} // namespace string_utils

// Mock utilities implementation
namespace mock_utils {

TestProjectData createTestProjectData(const std::string& projectName) {
    TestProjectData data;
    data.name = projectName;
    data.description = "Test project for unit testing";

    data.expectedFiles = {
        "README.md",
        "CMakeLists.txt",
        ".gitignore"
    };

    data.expectedDirs = {
        "src",
        "include",
        "tests",
        "docs"
    };

    return data;
}

} // namespace mock_utils

} // namespace test_utils
