#include <gtest/gtest.h>
#include "cli/cli_parser.h"
#include "templates/template_manager.h"
#include "utils/core/file_utils.h"
#include <filesystem>
#include <fstream>

using namespace utils;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "integration_test";
        std::filesystem::create_directories(testDir);
        
        // Change to test directory
        originalDir = std::filesystem::current_path();
        std::filesystem::current_path(testDir);
    }

    void TearDown() override {
        // Restore original directory
        std::filesystem::current_path(originalDir);
        
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;
    std::filesystem::path originalDir;
    
    // Helper to verify basic project structure
    void verifyBasicProjectStructure(const std::string& projectName) {
        EXPECT_TRUE(FileUtils::directoryExists(projectName));
        EXPECT_TRUE(FileUtils::directoryExists(projectName + "/src"));
        EXPECT_TRUE(FileUtils::directoryExists(projectName + "/include"));
        EXPECT_TRUE(FileUtils::fileExists(projectName + "/README.md"));
        EXPECT_TRUE(FileUtils::fileExists(projectName + "/CMakeLists.txt"));
    }
    
    // Helper to verify file contains expected content
    void verifyFileContains(const std::string& filePath, const std::string& expectedContent) {
        EXPECT_TRUE(FileUtils::fileExists(filePath));
        std::string content = FileUtils::readFromFile(filePath);
        EXPECT_TRUE(content.find(expectedContent) != std::string::npos) 
            << "File " << filePath << " does not contain: " << expectedContent;
    }
};

TEST_F(IntegrationTest, EndToEndConsoleProjectCreation) {
    // Simulate command line: cpp-scaffold create test-console --template console --build cmake
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-console", 
        "--template", "console", 
        "--build", "cmake",
        "--package", "none",
        "--no-git"  // Avoid git operations in tests
    };
    
    // Convert to argc/argv
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    // Parse command line
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    // Verify parsing
    EXPECT_EQ("test-console", options.projectName);
    EXPECT_EQ(TemplateType::Console, options.templateType);
    EXPECT_EQ(BuildSystem::CMake, options.buildSystem);
    EXPECT_EQ(PackageManager::None, options.packageManager);
    EXPECT_FALSE(options.initGit);
    
    // Create project
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify project structure
    verifyBasicProjectStructure("test-console");
    EXPECT_TRUE(FileUtils::fileExists("test-console/src/main.cpp"));
    
    // Verify CMakeLists.txt contains project name
    verifyFileContains("test-console/CMakeLists.txt", "test-console");
    
    // Verify README contains project name
    verifyFileContains("test-console/README.md", "test-console");
}

TEST_F(IntegrationTest, EndToEndLibraryProjectWithTests) {
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-library",
        "--template", "lib",
        "--build", "cmake",
        "--tests", "gtest",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    EXPECT_EQ("test-library", options.projectName);
    EXPECT_EQ(TemplateType::Lib, options.templateType);
    EXPECT_TRUE(options.includeTests);
    EXPECT_EQ(TestFramework::GTest, options.testFramework);
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify library structure
    verifyBasicProjectStructure("test-library");
    EXPECT_TRUE(FileUtils::directoryExists("test-library/include/test-library"));
    EXPECT_TRUE(FileUtils::directoryExists("test-library/tests"));
    EXPECT_TRUE(FileUtils::fileExists("test-library/tests/test_test-library.cpp"));
    
    // Verify CMakeLists.txt includes test configuration
    verifyFileContains("test-library/CMakeLists.txt", "gtest");
}

TEST_F(IntegrationTest, EndToEndGuiProjectWithDocumentation) {
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-gui",
        "--template", "gui",
        "--build", "cmake",
        "--docs",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    EXPECT_EQ("test-gui", options.projectName);
    EXPECT_EQ(TemplateType::Gui, options.templateType);
    EXPECT_TRUE(options.includeDocumentation);
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify GUI structure
    verifyBasicProjectStructure("test-gui");
    EXPECT_TRUE(FileUtils::directoryExists("test-gui/docs"));
    
    // Verify GUI-specific files
    EXPECT_TRUE(FileUtils::fileExists("test-gui/src/main.cpp"));
    verifyFileContains("test-gui/src/main.cpp", "QApplication");
}

TEST_F(IntegrationTest, EndToEndNetworkProjectWithCustomLibrary) {
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-network",
        "--template", "network",
        "--build", "cmake",
        "--network-lib", "boost",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    EXPECT_EQ("test-network", options.projectName);
    EXPECT_EQ(TemplateType::Network, options.templateType);
    EXPECT_TRUE(options.networkLibrary.has_value());
    EXPECT_EQ("boost", options.networkLibrary.value());
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify network structure
    verifyBasicProjectStructure("test-network");
    
    // Verify network-specific content
    verifyFileContains("test-network/CMakeLists.txt", "boost");
}

TEST_F(IntegrationTest, EndToEndProjectWithMesonBuildSystem) {
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-meson",
        "--template", "console",
        "--build", "meson",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    EXPECT_EQ(BuildSystem::Meson, options.buildSystem);
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Meson build file
    EXPECT_TRUE(FileUtils::fileExists("test-meson/meson.build"));
    verifyFileContains("test-meson/meson.build", "test-meson");
}

TEST_F(IntegrationTest, EndToEndProjectWithBazelBuildSystem) {
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-bazel",
        "--template", "console",
        "--build", "bazel",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    EXPECT_EQ(BuildSystem::Bazel, options.buildSystem);
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Bazel build files
    EXPECT_TRUE(FileUtils::fileExists("test-bazel/BUILD"));
    EXPECT_TRUE(FileUtils::fileExists("test-bazel/WORKSPACE"));
}

TEST_F(IntegrationTest, EndToEndProjectWithCodeStyle) {
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "test-style",
        "--template", "console",
        "--code-style",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    EXPECT_TRUE(options.includeCodeStyleTools);
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify code style files
    EXPECT_TRUE(FileUtils::fileExists("test-style/.clang-format"));
}

TEST_F(IntegrationTest, EndToEndComplexProject) {
    // Test a complex project with multiple features
    std::vector<std::string> args = {
        "cpp-scaffold", "create", "complex-project",
        "--template", "lib",
        "--build", "cmake",
        "--tests", "catch2",
        "--docs",
        "--code-style",
        "--package", "none",
        "--no-git"
    };
    
    std::vector<char*> argv;
    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    
    CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
    
    // Verify all options are parsed correctly
    EXPECT_EQ("complex-project", options.projectName);
    EXPECT_EQ(TemplateType::Lib, options.templateType);
    EXPECT_EQ(BuildSystem::CMake, options.buildSystem);
    EXPECT_TRUE(options.includeTests);
    EXPECT_EQ(TestFramework::Catch2, options.testFramework);
    EXPECT_TRUE(options.includeDocumentation);
    EXPECT_TRUE(options.includeCodeStyleTools);
    
    TemplateManager manager;
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify all features are present
    verifyBasicProjectStructure("complex-project");
    EXPECT_TRUE(FileUtils::directoryExists("complex-project/tests"));
    EXPECT_TRUE(FileUtils::directoryExists("complex-project/docs"));
    EXPECT_TRUE(FileUtils::fileExists("complex-project/.clang-format"));
    
    // Verify test framework
    verifyFileContains("complex-project/CMakeLists.txt", "Catch2");
}

TEST_F(IntegrationTest, ValidationFailsForInvalidOptions) {
    CliOptions options;
    options.projectName = ""; // Invalid: empty name
    
    EXPECT_FALSE(CliParser::validateOptions(options));
}

TEST_F(IntegrationTest, ValidationSucceedsForValidOptions) {
    CliOptions options;
    options.projectName = "valid-project";
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::None;
    
    EXPECT_TRUE(CliParser::validateOptions(options));
}

TEST_F(IntegrationTest, NetworkProjectGetsDefaultLibrary) {
    CliOptions options;
    options.projectName = "network-project";
    options.templateType = TemplateType::Network;
    // Don't set networkLibrary - should get default
    
    EXPECT_TRUE(CliParser::validateOptions(options));
    EXPECT_TRUE(options.networkLibrary.has_value());
    EXPECT_EQ("asio", options.networkLibrary.value());
}

TEST_F(IntegrationTest, TestProjectGetsDefaultFramework) {
    CliOptions options;
    options.projectName = "test-project";
    options.includeTests = true;
    options.testFramework = TestFramework::None; // Should get default
    
    EXPECT_TRUE(CliParser::validateOptions(options));
    EXPECT_EQ(TestFramework::GTest, options.testFramework);
}
