#include <gtest/gtest.h>
#include "templates/template_manager.h"
#include "cli/types/cli_options.h"
#include "utils/core/file_utils.h"
#include <filesystem>

using namespace templates;
using namespace cli;
using namespace utils;

class XMakeIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean up any existing test directories
        cleanupTestDirectories();
    }

    void TearDown() override {
        // Clean up test directories after each test
        cleanupTestDirectories();
    }

    void cleanupTestDirectories() {
        std::vector<std::string> testDirs = {
            "xmake-console-test",
            "xmake-lib-test", 
            "xmake-gui-test",
            "xmake-header-test",
            "xmake-modules-test",
            "xmake-webservice-test",
            "xmake-gameengine-test"
        };
        
        for (const auto& dir : testDirs) {
            if (FileUtils::directoryExists(dir)) {
                FileUtils::removeDirectory(dir);
            }
        }
    }

    CliOptions createXMakeOptions(const std::string& projectName, TemplateType templateType) {
        CliOptions options;
        options.projectName = projectName;
        options.templateType = templateType;
        options.buildSystem = BuildSystem::XMake;
        options.packageManager = PackageManager::None;
        options.testFramework = TestFramework::GTest;
        options.includeTests = true;
        options.includeDocs = false;
        options.includeCI = false;
        options.includeFormatting = false;
        options.includeBenchmarks = false;
        return options;
    }
};

TEST_F(XMakeIntegrationTest, ConsoleProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-console-test", TemplateType::Console);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-console-test/xmake.lua"));
    
    // Verify project structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-console-test/src"));
    EXPECT_TRUE(FileUtils::fileExists("xmake-console-test/src/main.cpp"));
    
    // Verify test structure when tests are enabled
    EXPECT_TRUE(FileUtils::directoryExists("xmake-console-test/tests"));
    EXPECT_TRUE(FileUtils::fileExists("xmake-console-test/tests/test_main.cpp"));
}

TEST_F(XMakeIntegrationTest, LibraryProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-lib-test", TemplateType::Lib);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-lib-test/xmake.lua"));
    
    // Verify library project structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-lib-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-lib-test/include"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-lib-test/examples"));
    
    // Verify library-specific files
    EXPECT_TRUE(FileUtils::fileExists("xmake-lib-test/include/xmake-lib-test/xmake-lib-test.h"));
    EXPECT_TRUE(FileUtils::fileExists("xmake-lib-test/src/xmake-lib-test.cpp"));
    EXPECT_TRUE(FileUtils::fileExists("xmake-lib-test/examples/example.cpp"));
}

TEST_F(XMakeIntegrationTest, GuiProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-gui-test", TemplateType::Gui);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-gui-test/xmake.lua"));
    
    // Verify GUI project structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-gui-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-gui-test/include"));
    
    // Verify GUI-specific files
    EXPECT_TRUE(FileUtils::fileExists("xmake-gui-test/src/main.cpp"));
}

TEST_F(XMakeIntegrationTest, HeaderOnlyLibProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-header-test", TemplateType::HeaderOnlyLib);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-header-test/xmake.lua"));
    
    // Verify header-only library structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-header-test/include"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-header-test/examples"));
    
    // Verify header-only specific files
    EXPECT_TRUE(FileUtils::fileExists("xmake-header-test/include/xmake-header-test/xmake-header-test.h"));
    EXPECT_TRUE(FileUtils::fileExists("xmake-header-test/examples/example.cpp"));
}

TEST_F(XMakeIntegrationTest, ModulesProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-modules-test", TemplateType::Modules);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-modules-test/xmake.lua"));
    
    // Verify modules project structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-modules-test/modules"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-modules-test/src"));
    
    // Verify modules-specific files
    EXPECT_TRUE(FileUtils::fileExists("xmake-modules-test/src/main.cpp"));
}

TEST_F(XMakeIntegrationTest, WebServiceProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-webservice-test", TemplateType::WebService);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-webservice-test/xmake.lua"));
    
    // Verify web service project structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-webservice-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-webservice-test/include"));
    
    // Verify web service-specific files
    EXPECT_TRUE(FileUtils::fileExists("xmake-webservice-test/src/main.cpp"));
}

TEST_F(XMakeIntegrationTest, GameEngineProjectWithXMake) {
    TemplateManager manager;
    auto options = createXMakeOptions("xmake-gameengine-test", TemplateType::GameEngine);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify XMake configuration file
    EXPECT_TRUE(FileUtils::fileExists("xmake-gameengine-test/xmake.lua"));
    
    // Verify game engine project structure
    EXPECT_TRUE(FileUtils::directoryExists("xmake-gameengine-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("xmake-gameengine-test/include"));
    
    // Verify game engine-specific files
    EXPECT_TRUE(FileUtils::fileExists("xmake-gameengine-test/src/main.cpp"));
}
