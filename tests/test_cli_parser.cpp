#include <gtest/gtest.h>
#include "../src/cli/cli_parser.h"
#include <vector>
#include <string>
#include <algorithm>

class CliParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to create argc/argv from vector of strings
    std::pair<int, char**> createArgv(const std::vector<std::string>& args) {
        char** argv = new char*[args.size()];
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i] = new char[args[i].length() + 1];
            strcpy(argv[i], args[i].c_str());
        }
        return {static_cast<int>(args.size()), argv};
    }
    
    void freeArgv(int argc, char** argv) {
        for (int i = 0; i < argc; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
};

TEST_F(CliParserTest, EnumConversions) {
    // Test template type conversions
    EXPECT_EQ("console", enums::to_string(TemplateType::Console));
    EXPECT_EQ("lib", enums::to_string(TemplateType::Lib));
    EXPECT_EQ("gui", enums::to_string(TemplateType::Gui));
    EXPECT_EQ("network", enums::to_string(TemplateType::Network));
    
    auto templateType = enums::to_template_type("console");
    ASSERT_TRUE(templateType.has_value());
    EXPECT_EQ(TemplateType::Console, templateType.value());
    
    templateType = enums::to_template_type("invalid");
    EXPECT_FALSE(templateType.has_value());
}

TEST_F(CliParserTest, BuildSystemConversions) {
    EXPECT_EQ("cmake", enums::to_string(BuildSystem::CMake));
    EXPECT_EQ("meson", enums::to_string(BuildSystem::Meson));
    EXPECT_EQ("bazel", enums::to_string(BuildSystem::Bazel));
    
    auto buildSystem = enums::to_build_system("cmake");
    ASSERT_TRUE(buildSystem.has_value());
    EXPECT_EQ(BuildSystem::CMake, buildSystem.value());
    
    buildSystem = enums::to_build_system("invalid");
    EXPECT_FALSE(buildSystem.has_value());
}

TEST_F(CliParserTest, PackageManagerConversions) {
    EXPECT_EQ("vcpkg", enums::to_string(PackageManager::Vcpkg));
    EXPECT_EQ("conan", enums::to_string(PackageManager::Conan));
    EXPECT_EQ("none", enums::to_string(PackageManager::None));
    
    auto packageManager = enums::to_package_manager("vcpkg");
    ASSERT_TRUE(packageManager.has_value());
    EXPECT_EQ(PackageManager::Vcpkg, packageManager.value());
}

TEST_F(CliParserTest, TestFrameworkConversions) {
    EXPECT_EQ("gtest", enums::to_string(TestFramework::GTest));
    EXPECT_EQ("catch2", enums::to_string(TestFramework::Catch2));
    EXPECT_EQ("doctest", enums::to_string(TestFramework::Doctest));
    
    auto testFramework = enums::to_test_framework("gtest");
    ASSERT_TRUE(testFramework.has_value());
    EXPECT_EQ(TestFramework::GTest, testFramework.value());
}

TEST_F(CliParserTest, LanguageConversions) {
    EXPECT_EQ("en", enums::to_string(Language::English));
    EXPECT_EQ("zh", enums::to_string(Language::Chinese));
    EXPECT_EQ("es", enums::to_string(Language::Spanish));
    
    auto language = enums::to_language("en");
    ASSERT_TRUE(language.has_value());
    EXPECT_EQ(Language::English, language.value());
}

TEST_F(CliParserTest, HelpCommand) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "help"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_TRUE(options.showHelp);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, VersionCommand) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "version"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_TRUE(options.version);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, BasicProjectCreation) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_EQ(TemplateType::Console, options.templateType);  // Default
    EXPECT_EQ(BuildSystem::CMake, options.buildSystem);     // Default
    EXPECT_EQ(PackageManager::Vcpkg, options.packageManager); // Default
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithTemplate) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-lib", "--template", "lib"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-lib", options.projectName);
    EXPECT_EQ(TemplateType::Lib, options.templateType);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithBuildSystem) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--build", "meson"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_EQ(BuildSystem::Meson, options.buildSystem);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithPackageManager) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--package", "conan"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_EQ(PackageManager::Conan, options.packageManager);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithTests) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--tests", "catch2"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_TRUE(options.includeTests);
    EXPECT_EQ(TestFramework::Catch2, options.testFramework);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithDocumentation) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--docs"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_TRUE(options.includeDocumentation);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithCodeStyle) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--code-style"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_TRUE(options.includeCodeStyleTools);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithNoGit) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--no-git"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_FALSE(options.initGit);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithVerbose) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--verbose"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_TRUE(options.verbose);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ProjectWithLanguage) {
    auto [argc, argv] = createArgv({"cpp-scaffold", "create", "my-project", "--language", "zh"});
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("my-project", options.projectName);
    EXPECT_EQ(Language::Chinese, options.language);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, ComplexProjectConfiguration) {
    auto [argc, argv] = createArgv({
        "cpp-scaffold", "create", "complex-project",
        "--template", "gui",
        "--build", "cmake",
        "--package", "vcpkg",
        "--tests", "gtest",
        "--docs",
        "--code-style",
        "--verbose"
    });
    
    CliOptions options = CliParser::parse(argc, argv);
    EXPECT_EQ("complex-project", options.projectName);
    EXPECT_EQ(TemplateType::Gui, options.templateType);
    EXPECT_EQ(BuildSystem::CMake, options.buildSystem);
    EXPECT_EQ(PackageManager::Vcpkg, options.packageManager);
    EXPECT_TRUE(options.includeTests);
    EXPECT_EQ(TestFramework::GTest, options.testFramework);
    EXPECT_TRUE(options.includeDocumentation);
    EXPECT_TRUE(options.includeCodeStyleTools);
    EXPECT_TRUE(options.verbose);
    
    freeArgv(argc, argv);
}

TEST_F(CliParserTest, OptionsValidation) {
    CliOptions options;
    options.projectName = "test-project";
    options.templateType = TemplateType::Network;
    options.includeTests = true;
    
    // Should set default network library and test framework
    EXPECT_TRUE(CliParser::validateOptions(options));
    EXPECT_TRUE(options.networkLibrary.has_value());
    EXPECT_EQ("asio", options.networkLibrary.value());
    EXPECT_EQ(TestFramework::GTest, options.testFramework);
}

TEST_F(CliParserTest, EmptyProjectNameValidation) {
    CliOptions options;
    options.projectName = "";
    
    EXPECT_FALSE(CliParser::validateOptions(options));
}

TEST_F(CliParserTest, AllEnumValues) {
    // Test that all enum values are accessible
    auto templateTypes = enums::all_template_types();
    EXPECT_FALSE(templateTypes.empty());
    EXPECT_TRUE(std::find(templateTypes.begin(), templateTypes.end(), std::string_view("console")) != templateTypes.end());

    auto buildSystems = enums::all_build_systems();
    EXPECT_FALSE(buildSystems.empty());
    EXPECT_TRUE(std::find(buildSystems.begin(), buildSystems.end(), std::string_view("cmake")) != buildSystems.end());

    auto packageManagers = enums::all_package_managers();
    EXPECT_FALSE(packageManagers.empty());
    EXPECT_TRUE(std::find(packageManagers.begin(), packageManagers.end(), std::string_view("vcpkg")) != packageManagers.end());
}
