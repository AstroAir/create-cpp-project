#include <gtest/gtest.h>
#include "templates/template_manager.h"
#include "cli/types/cli_options.h"
#include "utils/file_utils.h"
#include <filesystem>

using namespace templates;
using namespace cli;
using namespace utils;

class PremakeIntegrationTest : public ::testing::Test {
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
            "premake-console-test",
            "premake-lib-test", 
            "premake-gui-test",
            "premake-header-test",
            "premake-modules-test",
            "premake-webservice-test",
            "premake-gameengine-test"
        };
        
        for (const auto& dir : testDirs) {
            if (FileUtils::directoryExists(dir)) {
                FileUtils::removeDirectory(dir);
            }
        }
    }

    CliOptions createPremakeOptions(const std::string& projectName, TemplateType templateType) {
        CliOptions options;
        options.projectName = projectName;
        options.templateType = templateType;
        options.buildSystem = BuildSystem::Premake;
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

TEST_F(PremakeIntegrationTest, ConsoleProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-console-test", TemplateType::Console);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-console-test/premake5.lua"));
    
    // Verify project structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-console-test/src"));
    EXPECT_TRUE(FileUtils::fileExists("premake-console-test/src/main.cpp"));
    
    // Verify test structure when tests are enabled
    EXPECT_TRUE(FileUtils::directoryExists("premake-console-test/tests"));
    EXPECT_TRUE(FileUtils::fileExists("premake-console-test/tests/test_main.cpp"));
}

TEST_F(PremakeIntegrationTest, LibraryProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-lib-test", TemplateType::Lib);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-lib-test/premake5.lua"));
    
    // Verify library project structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-lib-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-lib-test/include"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-lib-test/examples"));
    
    // Verify library-specific files
    EXPECT_TRUE(FileUtils::fileExists("premake-lib-test/include/premake-lib-test/premake-lib-test.h"));
    EXPECT_TRUE(FileUtils::fileExists("premake-lib-test/src/premake-lib-test.cpp"));
    EXPECT_TRUE(FileUtils::fileExists("premake-lib-test/examples/example.cpp"));
}

TEST_F(PremakeIntegrationTest, GuiProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-gui-test", TemplateType::Gui);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-gui-test/premake5.lua"));
    
    // Verify GUI project structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-gui-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-gui-test/include"));
    
    // Verify GUI-specific files
    EXPECT_TRUE(FileUtils::fileExists("premake-gui-test/src/main.cpp"));
}

TEST_F(PremakeIntegrationTest, HeaderOnlyLibProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-header-test", TemplateType::HeaderOnlyLib);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-header-test/premake5.lua"));
    
    // Verify header-only library structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-header-test/include"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-header-test/examples"));
    
    // Verify header-only specific files
    EXPECT_TRUE(FileUtils::fileExists("premake-header-test/include/premake-header-test/premake-header-test.h"));
    EXPECT_TRUE(FileUtils::fileExists("premake-header-test/examples/example.cpp"));
}

TEST_F(PremakeIntegrationTest, ModulesProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-modules-test", TemplateType::Modules);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-modules-test/premake5.lua"));
    
    // Verify modules project structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-modules-test/modules"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-modules-test/src"));
    
    // Verify modules-specific files
    EXPECT_TRUE(FileUtils::fileExists("premake-modules-test/src/main.cpp"));
}

TEST_F(PremakeIntegrationTest, WebServiceProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-webservice-test", TemplateType::WebService);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-webservice-test/premake5.lua"));
    
    // Verify web service project structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-webservice-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-webservice-test/include"));
    
    // Verify web service-specific files
    EXPECT_TRUE(FileUtils::fileExists("premake-webservice-test/src/main.cpp"));
}

TEST_F(PremakeIntegrationTest, GameEngineProjectWithPremake) {
    TemplateManager manager;
    auto options = createPremakeOptions("premake-gameengine-test", TemplateType::GameEngine);
    
    EXPECT_TRUE(manager.createProject(options));
    
    // Verify Premake configuration file
    EXPECT_TRUE(FileUtils::fileExists("premake-gameengine-test/premake5.lua"));
    
    // Verify game engine project structure
    EXPECT_TRUE(FileUtils::directoryExists("premake-gameengine-test/src"));
    EXPECT_TRUE(FileUtils::directoryExists("premake-gameengine-test/include"));
    
    // Verify game engine-specific files
    EXPECT_TRUE(FileUtils::fileExists("premake-gameengine-test/src/main.cpp"));
}
