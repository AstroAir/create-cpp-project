#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "../src/templates/console_template.h"
#include "../src/templates/library_template.h"
#include "../src/templates/network_template.h"
#include "../src/templates/template_manager.h"
#include "../../src/utils/core/file_utils.h"

using namespace utils;

class TemplateEdgeCasesTest : public ::testing::Test {
   protected:
    void SetUp() override {
        testDir = std::filesystem::temp_directory_path() / "cpp_scaffold_edge_test";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        std::filesystem::current_path(testDir);
    }

    void TearDown() override {
        std::filesystem::current_path(std::filesystem::temp_directory_path());
        std::filesystem::remove_all(testDir);
    }

    std::filesystem::path testDir;
};

// Test template creation with invalid project names
TEST_F(TemplateEdgeCasesTest, InvalidProjectNames) {
    TemplateManager manager;

    // Test empty project name
    CliOptions options;
    options.projectName = "";
    options.templateType = TemplateType::Console;
    EXPECT_FALSE(manager.createProject(options));

    // Test project name with invalid characters
    options.projectName = "project/with\\invalid:chars";
    EXPECT_FALSE(manager.createProject(options));

    // Test very long project name
    options.projectName = std::string(300, 'a');
    EXPECT_FALSE(manager.createProject(options));
}

// Test template creation with existing directory
TEST_F(TemplateEdgeCasesTest, ExistingDirectory) {
    TemplateManager manager;

    // Create a directory first
    std::string projectName = "existing-project";
    std::filesystem::create_directory(projectName);

    CliOptions options;
    options.projectName = projectName;
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::None;
    options.initGit = false;

    // Should fail because directory already exists
    EXPECT_FALSE(manager.createProject(options));
}

// Test template creation with read-only parent directory
TEST_F(TemplateEdgeCasesTest, ReadOnlyParentDirectory) {
    TemplateManager manager;

    // Create a read-only directory
    std::string readOnlyDir = "readonly";
    std::filesystem::create_directory(readOnlyDir);

#ifndef _WIN32  // Skip on Windows as permissions work differently
    std::filesystem::permissions(readOnlyDir, std::filesystem::perms::owner_read |
                                                      std::filesystem::perms::group_read |
                                                      std::filesystem::perms::others_read);

    std::filesystem::current_path(readOnlyDir);

    CliOptions options;
    options.projectName = "test-project";
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::None;
    options.initGit = false;

    // Should fail because parent directory is read-only
    EXPECT_FALSE(manager.createProject(options));

    // Restore permissions for cleanup
    std::filesystem::current_path("..");
    std::filesystem::permissions(readOnlyDir, std::filesystem::perms::all);
#endif
}

// Test network template with all supported libraries
TEST_F(TemplateEdgeCasesTest, NetworkTemplateAllLibraries) {
    std::vector<std::string> networkLibs = {"asio", "boost", "poco"};

    for (const auto& lib : networkLibs) {
        CliOptions options;
        options.projectName = "test-network-" + lib;
        options.templateType = TemplateType::Network;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::None;
        options.networkLibrary = lib;
        options.initGit = false;

        NetworkTemplate networkTemplate(options);
        EXPECT_TRUE(networkTemplate.create()) << "Failed to create network template with " << lib;

        // Verify CMakeLists.txt contains the library
        std::string cmakeFile = options.projectName + "/CMakeLists.txt";
        EXPECT_TRUE(std::filesystem::exists(cmakeFile));

        std::string content = FileUtils::readFromFile(cmakeFile);
        EXPECT_FALSE(content.empty());

        // Clean up for next iteration
        std::filesystem::remove_all(options.projectName);
    }
}

// Test template with all build systems
TEST_F(TemplateEdgeCasesTest, AllBuildSystems) {
    std::vector<BuildSystem> buildSystems = {BuildSystem::CMake, BuildSystem::Meson,
                                             BuildSystem::Bazel, BuildSystem::XMake,
                                             BuildSystem::Premake};

    for (const auto& buildSystem : buildSystems) {
        CliOptions options;
        options.projectName = "test-build-" + std::to_string(static_cast<int>(buildSystem));
        options.templateType = TemplateType::Console;
        options.buildSystem = buildSystem;
        options.packageManager = PackageManager::None;
        options.initGit = false;

        TemplateManager manager;
        EXPECT_TRUE(manager.createProject(options))
                << "Failed to create project with build system " << static_cast<int>(buildSystem);

        // Verify appropriate build file exists
        std::string buildFile;
        switch (buildSystem) {
            case BuildSystem::CMake:
                buildFile = options.projectName + "/CMakeLists.txt";
                break;
            case BuildSystem::Meson:
                buildFile = options.projectName + "/meson.build";
                break;
            case BuildSystem::Bazel:
                buildFile = options.projectName + "/BUILD";
                break;
            case BuildSystem::XMake:
                buildFile = options.projectName + "/xmake.lua";
                break;
            case BuildSystem::Premake:
                buildFile = options.projectName + "/premake5.lua";
                break;
            case BuildSystem::Make:
                buildFile = options.projectName + "/Makefile";
                break;
            case BuildSystem::Ninja:
                buildFile = options.projectName + "/build.ninja";
                break;
        }

        EXPECT_TRUE(std::filesystem::exists(buildFile))
                << "Build file " << buildFile << " does not exist";

        // Clean up for next iteration
        std::filesystem::remove_all(options.projectName);
    }
}

// Test template with all test frameworks
TEST_F(TemplateEdgeCasesTest, AllTestFrameworks) {
    std::vector<TestFramework> testFrameworks = {TestFramework::GTest, TestFramework::Catch2,
                                                 TestFramework::Doctest, TestFramework::Boost};

    for (const auto& framework : testFrameworks) {
        CliOptions options;
        options.projectName = "test-framework-" + std::to_string(static_cast<int>(framework));
        options.templateType = TemplateType::Console;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::None;
        options.includeTests = true;
        options.testFramework = framework;
        options.initGit = false;

        TemplateManager manager;
        EXPECT_TRUE(manager.createProject(options))
                << "Failed to create project with test framework " << static_cast<int>(framework);

        // Verify test directory exists
        std::string testDir = options.projectName + "/tests";
        EXPECT_TRUE(std::filesystem::exists(testDir))
                << "Test directory does not exist for framework " << static_cast<int>(framework);

        // Clean up for next iteration
        std::filesystem::remove_all(options.projectName);
    }
}

// Test library template with different configurations
TEST_F(TemplateEdgeCasesTest, LibraryTemplateConfigurations) {
    // Test static library
    CliOptions options;
    options.projectName = "test-static-lib";
    options.templateType = TemplateType::Lib;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::None;
    options.includeTests = true;
    options.includeDocumentation = true;
    options.initGit = false;

    LibraryTemplate libTemplate(options);
    EXPECT_TRUE(libTemplate.create());

    // Verify all expected directories exist
    EXPECT_TRUE(std::filesystem::exists(options.projectName + "/src"));
    EXPECT_TRUE(std::filesystem::exists(options.projectName + "/include"));
    EXPECT_TRUE(std::filesystem::exists(options.projectName + "/tests"));
    EXPECT_TRUE(std::filesystem::exists(options.projectName + "/docs"));

    // Verify CMakeLists.txt has proper library configuration
    std::string cmakeContent = FileUtils::readFromFile(options.projectName + "/CMakeLists.txt");
    EXPECT_TRUE(cmakeContent.find("add_library") != std::string::npos);
    EXPECT_TRUE(cmakeContent.find("target_include_directories") != std::string::npos);
}

// Test XMake with all template types
TEST_F(TemplateEdgeCasesTest, XMakeWithAllTemplateTypes) {
    std::vector<TemplateType> templateTypes = {
            TemplateType::Console,   TemplateType::Lib,     TemplateType::HeaderOnlyLib,
            TemplateType::Gui,       TemplateType::Modules, TemplateType::WebService,
            TemplateType::GameEngine};

    for (const auto& templateType : templateTypes) {
        CliOptions options;
        options.projectName = "xmake-template-" + std::to_string(static_cast<int>(templateType));
        options.templateType = templateType;
        options.buildSystem = BuildSystem::XMake;
        options.packageManager = PackageManager::None;
        options.initGit = false;
        options.includeTests = true;

        TemplateManager manager;
        EXPECT_TRUE(manager.createProject(options))
                << "Failed to create XMake project with template type "
                << static_cast<int>(templateType);

        // Verify XMake file exists
        std::string projectPath = options.projectName;
        EXPECT_TRUE(FileUtils::fileExists(projectPath + "/xmake.lua"))
                << "XMake file missing for template type " << static_cast<int>(templateType);

        // Clean up for next iteration
        std::filesystem::remove_all(options.projectName);
    }
}

// Test Premake with all template types
TEST_F(TemplateEdgeCasesTest, PremakeWithAllTemplateTypes) {
    std::vector<TemplateType> templateTypes = {
            TemplateType::Console,   TemplateType::Lib,     TemplateType::HeaderOnlyLib,
            TemplateType::Gui,       TemplateType::Modules, TemplateType::WebService,
            TemplateType::GameEngine};

    for (const auto& templateType : templateTypes) {
        CliOptions options;
        options.projectName = "premake-template-" + std::to_string(static_cast<int>(templateType));
        options.templateType = templateType;
        options.buildSystem = BuildSystem::Premake;
        options.packageManager = PackageManager::None;
        options.initGit = false;
        options.includeTests = true;

        TemplateManager manager;
        EXPECT_TRUE(manager.createProject(options))
                << "Failed to create Premake project with template type "
                << static_cast<int>(templateType);

        // Verify Premake file exists
        std::string projectPath = options.projectName;
        EXPECT_TRUE(FileUtils::fileExists(projectPath + "/premake5.lua"))
                << "Premake file missing for template type " << static_cast<int>(templateType);

        // Clean up for next iteration
        std::filesystem::remove_all(options.projectName);
    }
}
