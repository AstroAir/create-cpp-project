#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>

namespace test_utils {

/**
 * @brief File system utilities for testing
 */
namespace fs_utils {
    /**
     * @brief Check if a directory contains all expected files
     * @param dir Directory to check
     * @param expectedFiles List of expected file names
     * @return true if all files exist
     */
    bool containsAllFiles(const std::filesystem::path& dir, const std::vector<std::string>& expectedFiles);

    /**
     * @brief Check if a directory contains all expected subdirectories
     * @param dir Directory to check
     * @param expectedDirs List of expected directory names
     * @return true if all directories exist
     */
    bool containsAllDirs(const std::filesystem::path& dir, const std::vector<std::string>& expectedDirs);

    /**
     * @brief Get all files in a directory with a specific extension
     * @param dir Directory to search
     * @param extension File extension (e.g., ".cpp", ".h")
     * @return Vector of file paths
     */
    std::vector<std::filesystem::path> getFilesWithExtension(const std::filesystem::path& dir, const std::string& extension);

    /**
     * @brief Count files in a directory (non-recursive)
     * @param dir Directory to count files in
     * @return Number of files
     */
    size_t countFiles(const std::filesystem::path& dir);

    /**
     * @brief Count directories in a directory (non-recursive)
     * @param dir Directory to count subdirectories in
     * @return Number of subdirectories
     */
    size_t countDirectories(const std::filesystem::path& dir);
}

/**
 * @brief String utilities for testing
 */
namespace string_utils {
    /**
     * @brief Check if a string contains all expected substrings
     * @param text Text to search in
     * @param expectedSubstrings List of substrings that should be present
     * @return true if all substrings are found
     */
    bool containsAll(const std::string& text, const std::vector<std::string>& expectedSubstrings);

    /**
     * @brief Check if a string matches a regex pattern
     * @param text Text to match
     * @param pattern Regex pattern
     * @return true if the text matches the pattern
     */
    bool matchesPattern(const std::string& text, const std::string& pattern);

    /**
     * @brief Split a string by delimiter
     * @param text Text to split
     * @param delimiter Delimiter to split by
     * @return Vector of split strings
     */
    std::vector<std::string> split(const std::string& text, const std::string& delimiter);

    /**
     * @brief Trim whitespace from both ends of a string
     * @param text Text to trim
     * @return Trimmed string
     */
    std::string trim(const std::string& text);

    /**
     * @brief Normalize line endings to \n
     * @param text Text to normalize
     * @return Text with normalized line endings
     */
    std::string normalizeLineEndings(const std::string& text);
}

/**
 * @brief Mock and test data utilities
 */
namespace mock_utils {
    /**
     * @brief Create a mock CLI options structure
     * @param projectName Name of the project
     * @return Mock CliOptions structure
     */
    // CliOptions createMockCliOptions(const std::string& projectName = "test_project");

    /**
     * @brief Create test project data
     * @param projectName Name of the project
     * @return Test project configuration
     */
    struct TestProjectData {
        std::string name;
        std::string description;
        std::vector<std::string> expectedFiles;
        std::vector<std::string> expectedDirs;
    };

    TestProjectData createTestProjectData(const std::string& projectName = "test_project");
}

/**
 * @brief Custom assertion macros for common test patterns
 */
#define EXPECT_FILE_EXISTS(path) \
    EXPECT_TRUE(std::filesystem::exists(path)) << "File does not exist: " << path

#define EXPECT_DIR_EXISTS(path) \
    EXPECT_TRUE(std::filesystem::exists(path) && std::filesystem::is_directory(path)) \
    << "Directory does not exist: " << path

#define EXPECT_FILE_CONTAINS(path, content) \
    do { \
        EXPECT_FILE_EXISTS(path); \
        std::ifstream file(path); \
        EXPECT_TRUE(file.is_open()) << "Could not open file: " << path; \
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); \
        EXPECT_TRUE(fileContent.find(content) != std::string::npos) \
        << "File " << path << " does not contain: " << content; \
    } while(0)

#define EXPECT_FILE_MATCHES_PATTERN(path, pattern) \
    do { \
        EXPECT_FILE_EXISTS(path); \
        std::ifstream file(path); \
        EXPECT_TRUE(file.is_open()) << "Could not open file: " << path; \
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); \
        EXPECT_TRUE(test_utils::string_utils::matchesPattern(fileContent, pattern)) \
        << "File " << path << " does not match pattern: " << pattern; \
    } while(0)

#define EXPECT_DIR_CONTAINS_FILES(dir, files) \
    EXPECT_TRUE(test_utils::fs_utils::containsAllFiles(dir, files)) \
    << "Directory " << dir << " does not contain all expected files"

#define EXPECT_DIR_CONTAINS_DIRS(dir, dirs) \
    EXPECT_TRUE(test_utils::fs_utils::containsAllDirs(dir, dirs)) \
    << "Directory " << dir << " does not contain all expected subdirectories"

} // namespace test_utils