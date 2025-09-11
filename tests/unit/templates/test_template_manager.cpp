#include <gtest/gtest.h>

#include <filesystem>

#include "templates/template_manager.h"
#include "utils/file_utils.h"

using namespace utils;

class TemplateManagerTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "template_manager_test";
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

    CliOptions createBasicOptions(const std::string& projectName, TemplateType templateType) {
        CliOptions options;
        options.projectName = projectName;
        options.templateType = templateType;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::None;  // Avoid external dependencies in tests
        options.includeTests = false;                   // Simplify for basic tests
        options.includeDocumentation = false;
        options.includeCodeStyleTools = false;
        options.initGit = false;  // Avoid git operations in tests
        options.verbose = false;
        return options;
    }
};

TEST_F(TemplateManagerTest, CreateConsoleProject) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-console", TemplateType::Console);

    EXPECT_TRUE(manager.createProject(options));

    // Verify project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-console"));
    EXPECT_TRUE(FileUtils::directoryExists("test-console/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-console/include"));
    EXPECT_TRUE(FileUtils::fileExists("test-console/CMakeLists.txt"));
    EXPECT_TRUE(FileUtils::fileExists("test-console/README.md"));
    EXPECT_TRUE(FileUtils::fileExists("test-console/src/main.cpp"));
}

TEST_F(TemplateManagerTest, CreateLibraryProject) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-lib", TemplateType::Lib);

    EXPECT_TRUE(manager.createProject(options));

    // Verify project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-lib"));
    EXPECT_TRUE(FileUtils::directoryExists("test-lib/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-lib/include"));
    EXPECT_TRUE(FileUtils::fileExists("test-lib/CMakeLists.txt"));
    EXPECT_TRUE(FileUtils::fileExists("test-lib/README.md"));

    // Library should have header files
    EXPECT_TRUE(FileUtils::directoryExists("test-lib/include/test-lib"));
}

TEST_F(TemplateManagerTest, CreateGuiProject) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-gui", TemplateType::Gui);

    EXPECT_TRUE(manager.createProject(options));

    // Verify project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-gui"));
    EXPECT_TRUE(FileUtils::directoryExists("test-gui/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-gui/include"));
    EXPECT_TRUE(FileUtils::fileExists("test-gui/CMakeLists.txt"));
    EXPECT_TRUE(FileUtils::fileExists("test-gui/README.md"));
    EXPECT_TRUE(FileUtils::fileExists("test-gui/src/main.cpp"));
}

TEST_F(TemplateManagerTest, CreateNetworkProject) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-network", TemplateType::Network);
    options.networkLibrary = "asio";  // Set network library

    EXPECT_TRUE(manager.createProject(options));

    // Verify project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-network"));
    EXPECT_TRUE(FileUtils::directoryExists("test-network/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-network/include"));
    EXPECT_TRUE(FileUtils::fileExists("test-network/CMakeLists.txt"));
    EXPECT_TRUE(FileUtils::fileExists("test-network/README.md"));
    EXPECT_TRUE(FileUtils::fileExists("test-network/src/main.cpp"));
}

TEST_F(TemplateManagerTest, CreateProjectWithTests) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-with-tests", TemplateType::Console);
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;

    EXPECT_TRUE(manager.createProject(options));

    // Verify test structure
    EXPECT_TRUE(FileUtils::directoryExists("test-with-tests"));
    EXPECT_TRUE(FileUtils::directoryExists("test-with-tests/tests"));
    EXPECT_TRUE(FileUtils::fileExists("test-with-tests/tests/test_main.cpp"));
}

TEST_F(TemplateManagerTest, CreateProjectWithDocumentation) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-with-docs", TemplateType::Lib);
    options.includeDocumentation = true;

    EXPECT_TRUE(manager.createProject(options));

    // Verify documentation structure
    EXPECT_TRUE(FileUtils::directoryExists("test-with-docs"));
    EXPECT_TRUE(FileUtils::directoryExists("test-with-docs/docs"));
}

TEST_F(TemplateManagerTest, CreateProjectWithCodeStyle) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-with-style", TemplateType::Console);
    options.includeCodeStyleTools = true;

    EXPECT_TRUE(manager.createProject(options));

    // Verify code style files
    EXPECT_TRUE(FileUtils::directoryExists("test-with-style"));
    EXPECT_TRUE(FileUtils::fileExists("test-with-style/.clang-format"));
}

TEST_F(TemplateManagerTest, CreateProjectWithMesonBuildSystem) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-meson", TemplateType::Console);
    options.buildSystem = BuildSystem::Meson;

    EXPECT_TRUE(manager.createProject(options));

    // Verify Meson build file
    EXPECT_TRUE(FileUtils::directoryExists("test-meson"));
    EXPECT_TRUE(FileUtils::fileExists("test-meson/meson.build"));
}

TEST_F(TemplateManagerTest, CreateProjectWithBazelBuildSystem) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-bazel", TemplateType::Console);
    options.buildSystem = BuildSystem::Bazel;

    EXPECT_TRUE(manager.createProject(options));

    // Verify Bazel build files
    EXPECT_TRUE(FileUtils::directoryExists("test-bazel"));
    EXPECT_TRUE(FileUtils::fileExists("test-bazel/BUILD"));
    EXPECT_TRUE(FileUtils::fileExists("test-bazel/WORKSPACE"));
}

TEST_F(TemplateManagerTest, CreateProjectWithXMakeBuildSystem) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-xmake", TemplateType::Console);
    options.buildSystem = BuildSystem::XMake;

    EXPECT_TRUE(manager.createProject(options));

    // Verify XMake build file
    EXPECT_TRUE(FileUtils::directoryExists("test-xmake"));
    EXPECT_TRUE(FileUtils::fileExists("test-xmake/xmake.lua"));

    // Verify XMake content contains expected elements
    std::string content = FileUtils::readFromFile("test-xmake/xmake.lua");
    EXPECT_TRUE(content.find("set_project(\"test-xmake\")") != std::string::npos);
    EXPECT_TRUE(content.find("set_languages(\"c++17\")") != std::string::npos);
    EXPECT_TRUE(content.find("target(\"test-xmake\")") != std::string::npos);
}

TEST_F(TemplateManagerTest, CreateProjectWithPremakeBuildSystem) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-premake", TemplateType::Console);
    options.buildSystem = BuildSystem::Premake;

    EXPECT_TRUE(manager.createProject(options));

    // Verify Premake build file
    EXPECT_TRUE(FileUtils::directoryExists("test-premake"));
    EXPECT_TRUE(FileUtils::fileExists("test-premake/premake5.lua"));

    // Verify Premake content contains expected elements
    std::string content = FileUtils::readFromFile("test-premake/premake5.lua");
    EXPECT_TRUE(content.find("workspace \"test-premake\"") != std::string::npos);
    EXPECT_TRUE(content.find("project \"test-premake\"") != std::string::npos);
    EXPECT_TRUE(content.find("cppdialect \"C++17\"") != std::string::npos);
}

TEST_F(TemplateManagerTest, CreateProjectWithVcpkgPackageManager) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-vcpkg", TemplateType::Console);
    options.packageManager = PackageManager::Vcpkg;

    EXPECT_TRUE(manager.createProject(options));

    // Verify vcpkg configuration
    EXPECT_TRUE(FileUtils::directoryExists("test-vcpkg"));
    EXPECT_TRUE(FileUtils::fileExists("test-vcpkg/vcpkg.json"));
}

TEST_F(TemplateManagerTest, CreateProjectWithConanPackageManager) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-conan", TemplateType::Console);
    options.packageManager = PackageManager::Conan;

    EXPECT_TRUE(manager.createProject(options));

    // Verify Conan configuration
    EXPECT_TRUE(FileUtils::directoryExists("test-conan"));
    EXPECT_TRUE(FileUtils::fileExists("test-conan/conanfile.txt"));
}

TEST_F(TemplateManagerTest, InvalidTemplateType) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-invalid", static_cast<TemplateType>(999));

    // This should fail gracefully
    EXPECT_FALSE(manager.createProject(options));
}

TEST_F(TemplateManagerTest, EmptyProjectName) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("", TemplateType::Console);

    // This should fail
    EXPECT_FALSE(manager.createProject(options));
}

TEST_F(TemplateManagerTest, ProjectNameWithSpecialCharacters) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-project_123", TemplateType::Console);

    EXPECT_TRUE(manager.createProject(options));
    EXPECT_TRUE(FileUtils::directoryExists("test-project_123"));
}

TEST_F(TemplateManagerTest, CreateMultipleProjects) {
    TemplateManager manager;

    // Create first project
    CliOptions options1 = createBasicOptions("project1", TemplateType::Console);
    EXPECT_TRUE(manager.createProject(options1));
    EXPECT_TRUE(FileUtils::directoryExists("project1"));

    // Create second project
    CliOptions options2 = createBasicOptions("project2", TemplateType::Lib);
    EXPECT_TRUE(manager.createProject(options2));
    EXPECT_TRUE(FileUtils::directoryExists("project2"));

    // Both should exist
    EXPECT_TRUE(FileUtils::directoryExists("project1"));
    EXPECT_TRUE(FileUtils::directoryExists("project2"));
}

TEST_F(TemplateManagerTest, OverwriteExistingProject) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("existing-project", TemplateType::Console);

    // Create project first time
    EXPECT_TRUE(manager.createProject(options));
    EXPECT_TRUE(FileUtils::directoryExists("existing-project"));

    // Try to create again - behavior depends on implementation
    // This test documents the current behavior
    bool result = manager.createProject(options);
    // The result may be true or false depending on implementation
    // The important thing is that it doesn't crash
    (void)result;  // Suppress unused variable warning
    EXPECT_TRUE(FileUtils::directoryExists("existing-project"));
}

// Comprehensive build system tests for different template types
TEST_F(TemplateManagerTest, LibraryTemplateWithXMake) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-lib-xmake", TemplateType::Lib);
    options.buildSystem = BuildSystem::XMake;

    EXPECT_TRUE(manager.createProject(options));

    // Verify XMake build file exists
    EXPECT_TRUE(FileUtils::fileExists("test-lib-xmake/xmake.lua"));

    // Verify basic project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-lib-xmake/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-lib-xmake/include"));
}

TEST_F(TemplateManagerTest, LibraryTemplateWithPremake) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-lib-premake", TemplateType::Lib);
    options.buildSystem = BuildSystem::Premake;

    EXPECT_TRUE(manager.createProject(options));

    // Verify Premake build file exists
    EXPECT_TRUE(FileUtils::fileExists("test-lib-premake/premake5.lua"));

    // Verify basic project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-lib-premake/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-lib-premake/include"));
}

TEST_F(TemplateManagerTest, GuiTemplateWithXMake) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-gui-xmake", TemplateType::Gui);
    options.buildSystem = BuildSystem::XMake;

    EXPECT_TRUE(manager.createProject(options));

    // Verify XMake build file exists
    EXPECT_TRUE(FileUtils::fileExists("test-gui-xmake/xmake.lua"));

    // Verify basic project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-gui-xmake/src"));
    EXPECT_TRUE(FileUtils::directoryExists("test-gui-xmake/include"));
}

TEST_F(TemplateManagerTest, HeaderOnlyLibTemplateWithXMake) {
    TemplateManager manager;
    CliOptions options = createBasicOptions("test-header-xmake", TemplateType::HeaderOnlyLib);
    options.buildSystem = BuildSystem::XMake;

    EXPECT_TRUE(manager.createProject(options));

    // Verify XMake build file exists
    EXPECT_TRUE(FileUtils::fileExists("test-header-xmake/xmake.lua"));

    // Verify basic project structure
    EXPECT_TRUE(FileUtils::directoryExists("test-header-xmake/include"));
    EXPECT_TRUE(FileUtils::directoryExists("test-header-xmake/examples"));
}
