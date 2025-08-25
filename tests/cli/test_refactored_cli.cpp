#include <gtest/gtest.h>
#include "../../src/cli/types/cli_enums.h"
#include "../../src/cli/types/cli_options.h"
#include "../../src/cli/parsing/argument_parser.h"
#include "../../src/cli/validation/cli_validator.h"
#include "../../src/cli/localization/localization.h"

// Test CLI enums functionality
class CliEnumsTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Setup for each test
  }
};

TEST_F(CliEnumsTest, TemplateTypeConversion) {
  // Test string to enum conversion
  auto consoleType = cli_enums::to_template_type("console");
  ASSERT_TRUE(consoleType.has_value());
  EXPECT_EQ(*consoleType, TemplateType::Console);
  
  auto libType = cli_enums::to_template_type("lib");
  ASSERT_TRUE(libType.has_value());
  EXPECT_EQ(*libType, TemplateType::Lib);
  
  // Test invalid conversion
  auto invalidType = cli_enums::to_template_type("invalid");
  EXPECT_FALSE(invalidType.has_value());
  
  // Test enum to string conversion
  std::string_view consoleStr = cli_enums::to_string(TemplateType::Console);
  EXPECT_EQ(consoleStr, "console");
  
  std::string_view libStr = cli_enums::to_string(TemplateType::Lib);
  EXPECT_EQ(libStr, "lib");
}

TEST_F(CliEnumsTest, BuildSystemConversion) {
  auto cmakeSystem = cli_enums::to_build_system("cmake");
  ASSERT_TRUE(cmakeSystem.has_value());
  EXPECT_EQ(*cmakeSystem, BuildSystem::CMake);
  
  auto mesonSystem = cli_enums::to_build_system("meson");
  ASSERT_TRUE(mesonSystem.has_value());
  EXPECT_EQ(*mesonSystem, BuildSystem::Meson);
  
  std::string_view cmakeStr = cli_enums::to_string(BuildSystem::CMake);
  EXPECT_EQ(cmakeStr, "cmake");
}

TEST_F(CliEnumsTest, PackageManagerConversion) {
  auto vcpkgManager = cli_enums::to_package_manager("vcpkg");
  ASSERT_TRUE(vcpkgManager.has_value());
  EXPECT_EQ(*vcpkgManager, PackageManager::Vcpkg);
  
  auto conanManager = cli_enums::to_package_manager("conan");
  ASSERT_TRUE(conanManager.has_value());
  EXPECT_EQ(*conanManager, PackageManager::Conan);
  
  std::string_view vcpkgStr = cli_enums::to_string(PackageManager::Vcpkg);
  EXPECT_EQ(vcpkgStr, "vcpkg");
}

TEST_F(CliEnumsTest, AllEnumValues) {
  // Test that all enum values are available
  auto templateTypes = cli_enums::all_template_types();
  EXPECT_FALSE(templateTypes.empty());
  EXPECT_TRUE(std::find(templateTypes.begin(), templateTypes.end(), "console") != templateTypes.end());
  
  auto buildSystems = cli_enums::all_build_systems();
  EXPECT_FALSE(buildSystems.empty());
  EXPECT_TRUE(std::find(buildSystems.begin(), buildSystems.end(), "cmake") != buildSystems.end());
}

// Test CLI validation functionality
class CliValidatorTest : public ::testing::Test {
protected:
  CliOptions createValidOptions() {
    CliOptions options;
    options.projectName = "test-project";
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    return options;
  }
};

TEST_F(CliValidatorTest, ValidConfiguration) {
  CliOptions options = createValidOptions();
  ValidationResult result = CliValidator::validate(options);
  
  EXPECT_TRUE(result.isValid);
  EXPECT_TRUE(result.errors.empty());
}

TEST_F(CliValidatorTest, InvalidProjectName) {
  CliOptions options = createValidOptions();
  options.projectName = ""; // Empty name should be invalid
  
  ValidationResult result = CliValidator::validate(options);
  
  EXPECT_FALSE(result.isValid);
  EXPECT_FALSE(result.errors.empty());
}

TEST_F(CliValidatorTest, ProjectNameValidation) {
  std::string error;
  
  // Valid names
  EXPECT_TRUE(CliValidator::validateProjectName("my-project", error));
  EXPECT_TRUE(CliValidator::validateProjectName("my_project", error));
  EXPECT_TRUE(CliValidator::validateProjectName("MyProject", error));
  
  // Invalid names
  EXPECT_FALSE(CliValidator::validateProjectName("", error));
  EXPECT_FALSE(CliValidator::validateProjectName("my project", error)); // Space
  EXPECT_FALSE(CliValidator::validateProjectName("123project", error)); // Starts with number
}

TEST_F(CliValidatorTest, EmailValidation) {
  std::string error;
  
  // Valid emails
  EXPECT_TRUE(CliValidator::validateEmail("user@example.com", error));
  EXPECT_TRUE(CliValidator::validateEmail("test.user+tag@domain.co.uk", error));
  
  // Invalid emails
  EXPECT_FALSE(CliValidator::validateEmail("", error));
  EXPECT_FALSE(CliValidator::validateEmail("invalid-email", error));
  EXPECT_FALSE(CliValidator::validateEmail("user@", error));
  EXPECT_FALSE(CliValidator::validateEmail("@domain.com", error));
}

TEST_F(CliValidatorTest, GitUrlValidation) {
  std::string error;
  
  // Valid URLs
  EXPECT_TRUE(CliValidator::validateGitUrl("https://github.com/user/repo.git", error));
  EXPECT_TRUE(CliValidator::validateGitUrl("git@github.com:user/repo.git", error));
  EXPECT_TRUE(CliValidator::validateGitUrl("ssh://git@server.com/repo.git", error));
  
  // Invalid URLs
  EXPECT_FALSE(CliValidator::validateGitUrl("", error));
  EXPECT_FALSE(CliValidator::validateGitUrl("not-a-url", error));
  EXPECT_FALSE(CliValidator::validateGitUrl("http://", error));
}

TEST_F(CliValidatorTest, NetworkTemplateValidation) {
  CliOptions options = createValidOptions();
  options.templateType = TemplateType::Network;
  // No network library specified
  
  ValidationResult result = CliValidator::validate(options);
  
  EXPECT_FALSE(result.isValid);
  EXPECT_TRUE(std::any_of(result.errors.begin(), result.errors.end(),
                          [](const std::string& error) {
                            return error.find("network library") != std::string::npos;
                          }));
}

TEST_F(CliValidatorTest, GuiTemplateValidation) {
  CliOptions options = createValidOptions();
  options.templateType = TemplateType::Gui;
  // No GUI frameworks specified
  
  ValidationResult result = CliValidator::validate(options);
  
  EXPECT_FALSE(result.isValid);
  EXPECT_TRUE(std::any_of(result.errors.begin(), result.errors.end(),
                          [](const std::string& error) {
                            return error.find("GUI framework") != std::string::npos;
                          }));
}

// Test argument parsing functionality
class ArgumentParserTest : public ::testing::Test {
protected:
  std::vector<std::string> createArgs(std::initializer_list<const char*> args) {
    return std::vector<std::string>(args);
  }
};

TEST_F(ArgumentParserTest, CommandDetection) {
  auto args = createArgs({"create", "my-project"});
  auto command = argument_parser::detectCommand(args);
  EXPECT_EQ(command, argument_parser::CommandType::Create);
  
  args = createArgs({"help"});
  command = argument_parser::detectCommand(args);
  EXPECT_EQ(command, argument_parser::CommandType::Help);
  
  args = createArgs({"version"});
  command = argument_parser::detectCommand(args);
  EXPECT_EQ(command, argument_parser::CommandType::Version);
}

TEST_F(ArgumentParserTest, ProjectNameExtraction) {
  auto args = createArgs({"create", "my-project"});
  auto command = argument_parser::detectCommand(args);
  std::string projectName = argument_parser::extractProjectName(args, command);
  EXPECT_EQ(projectName, "my-project");
  
  args = createArgs({"help"});
  command = argument_parser::detectCommand(args);
  projectName = argument_parser::extractProjectName(args, command);
  EXPECT_TRUE(projectName.empty());
}

TEST_F(ArgumentParserTest, CommaSeparatedListParsing) {
  auto result = argument_parser::parseCommaSeparatedList("item1,item2,item3");
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "item1");
  EXPECT_EQ(result[1], "item2");
  EXPECT_EQ(result[2], "item3");
  
  // Test with spaces
  result = argument_parser::parseCommaSeparatedList("item1, item2 , item3");
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "item1");
  EXPECT_EQ(result[1], "item2");
  EXPECT_EQ(result[2], "item3");
}

// Test localization functionality
class LocalizationTest : public ::testing::Test {
protected:
  void SetUp() override {
    Localization::initialize();
  }
};

TEST_F(LocalizationTest, BasicLocalization) {
  // Test English (default)
  Localization::setCurrentLanguage(Language::English);
  std::string_view welcome = Localization::get("welcomeMessage");
  EXPECT_FALSE(welcome.empty());
  EXPECT_TRUE(welcome.find("Welcome") != std::string_view::npos);
  
  // Test Chinese
  Localization::setCurrentLanguage(Language::Chinese);
  welcome = Localization::get("welcomeMessage");
  EXPECT_FALSE(welcome.empty());
  EXPECT_TRUE(welcome.find("欢迎") != std::string_view::npos);
}

TEST_F(LocalizationTest, FallbackToEnglish) {
  Localization::setCurrentLanguage(Language::Chinese);
  
  // Test with a key that might not exist in Chinese
  std::string_view result = Localization::get("nonexistentKey");
  EXPECT_EQ(result, "nonexistentKey"); // Should return the key itself as fallback
}

TEST_F(LocalizationTest, LanguageSwitch) {
  // Test switching languages
  Localization::setCurrentLanguage(Language::English);
  EXPECT_EQ(Localization::getCurrentLanguage(), Language::English);
  
  Localization::setCurrentLanguage(Language::Spanish);
  EXPECT_EQ(Localization::getCurrentLanguage(), Language::Spanish);
}

// Integration test for the complete CLI system
class CliIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    Localization::initialize();
  }
};

TEST_F(CliIntegrationTest, CompleteWorkflow) {
  // Test a complete workflow from parsing to validation
  CliOptions options;
  options.projectName = "integration-test";
  options.templateType = TemplateType::Console;
  options.buildSystem = BuildSystem::CMake;
  options.packageManager = PackageManager::Vcpkg;
  options.includeTests = true;
  options.testFramework = TestFramework::GTest;
  
  // Validate the configuration
  ValidationResult result = CliValidator::validate(options);
  EXPECT_TRUE(result.isValid);
  
  // Test enum conversions work correctly
  std::string_view templateStr = cli_enums::to_string(options.templateType);
  EXPECT_EQ(templateStr, "console");
  
  auto parsedTemplate = cli_enums::to_template_type(std::string(templateStr));
  ASSERT_TRUE(parsedTemplate.has_value());
  EXPECT_EQ(*parsedTemplate, options.templateType);
}
