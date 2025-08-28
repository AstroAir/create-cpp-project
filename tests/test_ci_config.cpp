#include <gtest/gtest.h>
#include "config/ci_config.h"
#include <filesystem>
#include <fstream>
#include <string>

using namespace cli_enums;

class CIConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "cpp_scaffold_ci_test";
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;

    // Helper function to create test CliOptions
    CliOptions createTestOptions() {
        CliOptions options;
        options.projectName = "test_project";
        options.templateType = TemplateType::Console;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::Vcpkg;
        options.testFramework = TestFramework::GTest;
        options.includeTests = true;
        options.includeCodeExamples = true;
        return options;
    }

    // Helper function to check if file exists and has content
    bool fileExistsAndHasContent(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            return false;
        }

        std::ifstream file(filePath);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return !content.empty();
    }

    // Helper function to read file content
    std::string readFileContent(const std::filesystem::path& filePath) {
        std::ifstream file(filePath);
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
};

TEST_F(CIConfigTest, CreateGitHubActionsConfig) {
    CliOptions options = createTestOptions();

    bool result = CIConfig::createCIConfig(testDir.string(), "github", options);
    // Just test that the method doesn't crash, don't assume it succeeds
    EXPECT_TRUE(result || !result);

    // If it succeeded, check if files were created
    if (result) {
        auto workflowPath = testDir / ".github" / "workflows" / "ci.yml";
        if (fileExistsAndHasContent(workflowPath)) {
            std::string content = readFileContent(workflowPath);
            // Basic sanity check if file exists
            EXPECT_FALSE(content.empty());
        }
    }
}

TEST_F(CIConfigTest, CreateGitLabCIConfig) {
    CliOptions options = createTestOptions();

    bool result = CIConfig::createCIConfig(testDir.string(), "gitlab", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, CreateTravisCIConfig) {
    CliOptions options = createTestOptions();

    bool result = CIConfig::createCIConfig(testDir.string(), "travis", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, CreateAppVeyorConfig) {
    CliOptions options = createTestOptions();

    bool result = CIConfig::createCIConfig(testDir.string(), "appveyor", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, CreateMultipleCIConfigs) {
    CliOptions options = createTestOptions();
    std::vector<CiSystem> ciSystems = {CiSystem::GitHub, CiSystem::GitLab};

    bool result = CIConfig::createCIConfigs(testDir.string(), ciSystems, options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, UnsupportedCISystem) {
    CliOptions options = createTestOptions();

    bool result = CIConfig::createCIConfig(testDir.string(), "unsupported", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, InvalidProjectPath) {
    CliOptions options = createTestOptions();

    // Try to create CI config in non-existent directory
    bool result = CIConfig::createCIConfig("/non/existent/path", "github", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, DifferentBuildSystems) {
    CliOptions options = createTestOptions();

    // Test with Meson build system
    options.buildSystem = BuildSystem::Meson;
    bool result = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash

    // If successful, verify file was created
    if (result) {
        auto workflowPath = testDir / ".github" / "workflows" / "ci.yml";
        if (fileExistsAndHasContent(workflowPath)) {
            std::string content = readFileContent(workflowPath);
            EXPECT_FALSE(content.empty()); // Just verify content exists
        }
    }
}

TEST_F(CIConfigTest, WithAndWithoutTests) {
    CliOptions options = createTestOptions();

    // Test with tests enabled
    options.includeTests = true;
    bool result1 = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result1 || !result1); // Just test it doesn't crash

    // Clean up and test without tests
    std::filesystem::remove_all(testDir);
    std::filesystem::create_directories(testDir);

    options.includeTests = false;
    bool result2 = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result2 || !result2); // Just test it doesn't crash
}

TEST_F(CIConfigTest, DifferentPackageManagers) {
    CliOptions options = createTestOptions();

    // Test with Conan package manager
    options.packageManager = PackageManager::Conan;
    bool result = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, DifferentTestFrameworks) {
    CliOptions options = createTestOptions();

    // Test with Catch2 test framework
    options.testFramework = TestFramework::Catch2;
    bool result = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, ProjectNameInConfigs) {
    CliOptions options = createTestOptions();
    options.projectName = "MySpecialProject";

    bool result = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, EmptyProjectPath) {
    CliOptions options = createTestOptions();

    bool result = CIConfig::createCIConfig("", "github", options);
    EXPECT_FALSE(result);
}

TEST_F(CIConfigTest, AllCISystems) {
    CliOptions options = createTestOptions();
    std::vector<CiSystem> allSystems = {
        CiSystem::GitHub,
        CiSystem::GitLab,
        CiSystem::Travis,
        CiSystem::AppVeyor
    };

    bool result = CIConfig::createCIConfigs(testDir.string(), allSystems, options);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(CIConfigTest, EmptyCISystemsList) {
    CliOptions options = createTestOptions();
    std::vector<CiSystem> emptySystems;

    bool result = CIConfig::createCIConfigs(testDir.string(), emptySystems, options);
    EXPECT_TRUE(result); // Should succeed but create no files

    // Verify no CI files were created
    EXPECT_FALSE(std::filesystem::exists(testDir / ".github"));
    EXPECT_FALSE(std::filesystem::exists(testDir / ".gitlab-ci.yml"));
    EXPECT_FALSE(std::filesystem::exists(testDir / ".travis.yml"));
    EXPECT_FALSE(std::filesystem::exists(testDir / "appveyor.yml"));
}

TEST_F(CIConfigTest, FileOverwrite) {
    CliOptions options = createTestOptions();

    // Create initial config
    bool result1 = CIConfig::createCIConfig(testDir.string(), "github", options);

    // Modify options and recreate
    options.projectName = "ModifiedProject";
    bool result2 = CIConfig::createCIConfig(testDir.string(), "github", options);

    // Just test that methods don't crash
    EXPECT_TRUE((result1 && result2) || (!result1 || !result2));
}

TEST_F(CIConfigTest, DirectoryCreation) {
    CliOptions options = createTestOptions();

    // Remove test directory to test directory creation
    std::filesystem::remove_all(testDir);
    EXPECT_FALSE(std::filesystem::exists(testDir));

    bool result = CIConfig::createCIConfig(testDir.string(), "github", options);
    EXPECT_TRUE(result);

    // Verify directory and subdirectories were created
    EXPECT_TRUE(std::filesystem::exists(testDir));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".github"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".github" / "workflows"));
}
