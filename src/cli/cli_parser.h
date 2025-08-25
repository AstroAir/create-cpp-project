#pragma once
#include "types/cli_options.h"
#include "types/cli_enums.h"
#include "localization/localization.h"
#include "../utils/terminal_utils.h" // 引入终端工具
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include <fmt/format.h>

// Import the enums and types from the types module
using namespace cli_enums;

// 枚举与字符串互相转换的命名空间
namespace enums {
// 将枚举转换为字符串
std::string_view to_string(TemplateType type);
std::string_view to_string(BuildSystem system);
std::string_view to_string(PackageManager manager);
std::string_view to_string(TestFramework framework);
std::string_view to_string(EditorConfig editor);
std::string_view to_string(CiSystem ci);
std::string_view to_string(Language lang);
std::string_view to_string(GitWorkflow workflow);
std::string_view to_string(GitBranchStrategy strategy);
std::string_view to_string(LicenseType license);

// 从字符串转换为枚举
std::optional<TemplateType> to_template_type(std::string_view str);
std::optional<BuildSystem> to_build_system(std::string_view str);
std::optional<PackageManager> to_package_manager(std::string_view str);
std::optional<TestFramework> to_test_framework(std::string_view str);
std::optional<EditorConfig> to_editor_config(std::string_view str);
std::optional<CiSystem> to_ci_system(std::string_view str);
std::optional<Language> to_language(std::string_view str);
std::optional<GitWorkflow> to_git_workflow(std::string_view str);
std::optional<GitBranchStrategy> to_git_branch_strategy(std::string_view str);
std::optional<LicenseType> to_license_type(std::string_view str);

// 获取所有值的字符串表示
std::vector<std::string_view> all_template_types();
std::vector<std::string_view> all_build_systems();
std::vector<std::string_view> all_package_managers();
std::vector<std::string_view> all_test_frameworks();
std::vector<std::string_view> all_editor_configs();
std::vector<std::string_view> all_ci_systems();
std::vector<std::string_view> all_languages();
std::vector<std::string_view> all_git_workflows();
std::vector<std::string_view> all_git_branch_strategies();
std::vector<std::string_view> all_license_types();

// New enum conversions for advanced configuration
std::string_view to_string(CppStandard standard);
std::optional<CppStandard> to_cpp_standard(std::string_view str);
std::string_view to_string(ProjectStructure structure);
std::optional<ProjectStructure> to_project_structure(std::string_view str);
std::string_view to_string(CompilerFlags flags);
std::optional<CompilerFlags> to_compiler_flags(std::string_view str);
std::vector<std::string_view> all_cpp_standards();
std::vector<std::string_view> all_project_structures();
std::vector<std::string_view> all_compiler_flags();

} // namespace enums

// CliOptions is now imported from types/cli_options.h

// UserInput class is now available in input/user_input.h
// ConfigManager class is now available in config/config_manager.h

// ProjectWizard class is now available in wizard/project_wizard.h

// CliParser class is now available in cli_parser_new.h

// fmt formatters are now imported from types/cli_enums.h
