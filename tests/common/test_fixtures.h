#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <memory>

namespace test_utils {

/**
 * @brief Base fixture for tests that need temporary directory management
 *
 * This fixture provides automatic creation and cleanup of temporary directories
 * for tests that need to work with the filesystem.
 */
class TempDirectoryFixture : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    /**
     * @brief Get the path to the temporary test directory
     * @return Path to the temporary directory
     */
    const std::filesystem::path& getTempDir() const { return testDir; }

    /**
     * @brief Create a subdirectory within the test directory
     * @param subdir Name of the subdirectory to create
     * @return Path to the created subdirectory
     */
    std::filesystem::path createSubDir(const std::string& subdir);

    /**
     * @brief Create a test file with specified content
     * @param filename Name of the file to create
     * @param content Content to write to the file
     * @return Path to the created file
     */
    std::filesystem::path createTestFile(const std::string& filename, const std::string& content = "");

private:
    std::filesystem::path testDir;
    std::filesystem::path originalDir;
};

/**
 * @brief Fixture for tests that need a basic project structure
 *
 * This fixture extends TempDirectoryFixture and creates a basic project
 * structure with common directories and files.
 */
class ProjectFixture : public TempDirectoryFixture {
protected:
    void SetUp() override;

    /**
     * @brief Get the project root directory
     * @return Path to the project root
     */
    const std::filesystem::path& getProjectDir() const { return projectDir; }

    /**
     * @brief Get the project name
     * @return Project name
     */
    const std::string& getProjectName() const { return projectName; }

    /**
     * @brief Verify that the basic project structure exists
     * @return true if all expected directories and files exist
     */
    bool verifyBasicProjectStructure() const;

    /**
     * @brief Verify that a file contains expected content
     * @param filePath Path to the file to check
     * @param expectedContent Content that should be present
     * @return true if the file contains the expected content
     */
    bool verifyFileContains(const std::filesystem::path& filePath, const std::string& expectedContent) const;

protected:
    std::filesystem::path projectDir;
    std::string projectName;
};

/**
 * @brief Fixture for tests that need configuration management
 *
 * This fixture provides common configuration setups and utilities
 * for testing configuration-related functionality.
 */
class ConfigFixture : public TempDirectoryFixture {
protected:
    void SetUp() override;

    /**
     * @brief Create a test configuration file
     * @param configName Name of the configuration
     * @param content Configuration content
     * @return Path to the created configuration file
     */
    std::filesystem::path createConfigFile(const std::string& configName, const std::string& content);

    /**
     * @brief Get the configuration directory
     * @return Path to the configuration directory
     */
    const std::filesystem::path& getConfigDir() const { return configDir; }

private:
    std::filesystem::path configDir;
};

/**
 * @brief Fixture for CLI-related tests
 *
 * This fixture provides utilities for testing command-line interface
 * components and argument parsing.
 */
class CliFixture : public TempDirectoryFixture {
protected:
    void SetUp() override;

    /**
     * @brief Create command line arguments for testing
     * @param args Vector of argument strings
     * @return Pair of argc and argv suitable for CLI parsing
     */
    std::pair<int, char**> createArgv(const std::vector<std::string>& args);

    /**
     * @brief Clean up allocated argv
     */
    void cleanupArgv();

private:
    std::vector<char*> allocatedArgs;
};

/**
 * @brief Fixture for template-related tests
 *
 * This fixture provides utilities for testing template creation
 * and management functionality.
 */
class TemplateFixture : public ProjectFixture {
protected:
    void SetUp() override;

    /**
     * @brief Create a basic CLI options structure for testing
     * @param projectName Name of the project
     * @param templateType Type of template to create
     * @return CliOptions structure with basic settings
     */
    // CliOptions createBasicOptions(const std::string& projectName, TemplateType templateType);

    /**
     * @brief Verify that template-specific files exist
     * @param templateType Type of template to verify
     * @return true if all expected template files exist
     */
    // bool verifyTemplateStructure(TemplateType templateType) const;
};

} // namespace test_utils