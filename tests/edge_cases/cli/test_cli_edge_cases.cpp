#include <gtest/gtest.h>
#include "../src/cli/cli_parser.h"
#include <vector>
#include <string>

class CliEdgeCasesTest : public ::testing::Test {
protected:
    std::vector<char*> createArgv(const std::vector<std::string>& args) {
        argv_storage.clear();
        argv_storage.reserve(args.size());
        
        for (const auto& arg : args) {
            argv_storage.push_back(const_cast<char*>(arg.c_str()));
        }
        
        return argv_storage;
    }
    
private:
    std::vector<char*> argv_storage;
};

// Test malformed command line arguments
TEST_F(CliEdgeCasesTest, MalformedArguments) {
    // Test missing value for option that requires one
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "test", "--template"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // Should use default template when value is missing
        EXPECT_EQ(TemplateType::Console, options.templateType);
    }
    
    // Test invalid template type
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "test", "--template", "invalid-template"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // Should use default template for invalid value
        EXPECT_EQ(TemplateType::Console, options.templateType);
    }
    
    // Test invalid build system
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "test", "--build", "invalid-build"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // Should use default build system for invalid value
        EXPECT_EQ(BuildSystem::CMake, options.buildSystem);
    }
}

// Test edge cases with project names
TEST_F(CliEdgeCasesTest, ProjectNameEdgeCases) {
    // Test project name with spaces (should be handled)
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "project with spaces"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_EQ("project with spaces", options.projectName);
    }
    
    // Test project name with special characters
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "project-name_123"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_EQ("project-name_123", options.projectName);
    }
    
    // Test very long project name
    {
        std::string longName(100, 'a');
        std::vector<std::string> args = {"cpp-scaffold", "create", longName};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_EQ(longName, options.projectName);
    }
}

// Test conflicting options
TEST_F(CliEdgeCasesTest, ConflictingOptions) {
    // Test --no-tests with --test-framework
    {
        std::vector<std::string> args = {
            "cpp-scaffold", "create", "test", 
            "--no-tests", "--test-framework", "gtest"
        };
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // --no-tests should take precedence
        EXPECT_FALSE(options.includeTests);
    }
    
    // Test --no-git with git-related options
    {
        std::vector<std::string> args = {
            "cpp-scaffold", "create", "test", 
            "--no-git", "--git-hooks"
        };
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // --no-git should take precedence
        EXPECT_FALSE(options.initGit);
    }
}

// Test boundary values
TEST_F(CliEdgeCasesTest, BoundaryValues) {
    // Test with maximum number of arguments
    {
        std::vector<std::string> args = {
            "cpp-scaffold", "create", "test-project",
            "--template", "lib",
            "--build", "cmake",
            "--package", "vcpkg",
            "--tests", "--test-framework", "gtest",
            "--docs",
            "--code-style",
            "--language", "english",
            "--verbose"
        };
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_EQ("test-project", options.projectName);
        EXPECT_EQ(TemplateType::Lib, options.templateType);
        EXPECT_EQ(BuildSystem::CMake, options.buildSystem);
        EXPECT_EQ(PackageManager::Vcpkg, options.packageManager);
        EXPECT_TRUE(options.includeTests);
        EXPECT_EQ(TestFramework::GTest, options.testFramework);
        EXPECT_TRUE(options.includeDocumentation);
        EXPECT_TRUE(options.includeCodeStyleTools);
        EXPECT_TRUE(options.verbose);
    }
}

// Test case sensitivity
TEST_F(CliEdgeCasesTest, CaseSensitivity) {
    // Test uppercase template names
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "test", "--template", "CONSOLE"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // Should handle case insensitively or use default
        EXPECT_TRUE(options.templateType == TemplateType::Console || 
                   options.templateType == TemplateType::Console);
    }
    
    // Test mixed case build system
    {
        std::vector<std::string> args = {"cpp-scaffold", "create", "test", "--build", "CMake"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // Should handle case insensitively or use default
        EXPECT_TRUE(options.buildSystem == BuildSystem::CMake);
    }
}

// Test option validation
TEST_F(CliEdgeCasesTest, OptionValidation) {
    // Test network template without network library
    {
        CliOptions options;
        options.projectName = "test-network";
        options.templateType = TemplateType::Network;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::None;
        
        // Should be valid - network library should get default value
        EXPECT_TRUE(CliParser::validateOptions(options));
        EXPECT_TRUE(options.networkLibrary.has_value());
    }
    
    // Test project with tests but no test framework
    {
        CliOptions options;
        options.projectName = "test-project";
        options.templateType = TemplateType::Console;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::None;
        options.includeTests = true;
        // testFramework not set
        
        // Should be valid - test framework should get default value
        EXPECT_TRUE(CliParser::validateOptions(options));
    }
}

// Test help and version commands
TEST_F(CliEdgeCasesTest, HelpAndVersionCommands) {
    // Test help command
    {
        std::vector<std::string> args = {"cpp-scaffold", "help"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_TRUE(options.showHelp);
    }
    
    // Test --help flag
    {
        std::vector<std::string> args = {"cpp-scaffold", "--help"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_TRUE(options.showHelp);
    }
    
    // Test version command
    {
        std::vector<std::string> args = {"cpp-scaffold", "version"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_TRUE(options.version);
    }
    
    // Test --version flag
    {
        std::vector<std::string> args = {"cpp-scaffold", "--version"};
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        EXPECT_TRUE(options.version);
    }
}

// Test unknown arguments
TEST_F(CliEdgeCasesTest, UnknownArguments) {
    // Test with unknown flags
    {
        std::vector<std::string> args = {
            "cpp-scaffold", "create", "test", 
            "--unknown-flag", "value",
            "--another-unknown"
        };
        auto argv = createArgv(args);
        
        CliOptions options = CliParser::parse(static_cast<int>(argv.size()), argv.data());
        // Should still parse known arguments correctly
        EXPECT_EQ("test", options.projectName);
        EXPECT_EQ(TemplateType::Console, options.templateType);
    }
}
