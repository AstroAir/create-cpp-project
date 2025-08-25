#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <fmt/format.h>

// Core project configuration enums
enum class TemplateType {
  Console,
  Lib,
  HeaderOnlyLib,
  MultiExecutable,
  Gui,
  Network,
  Embedded,
  WebService,
  GameEngine,
  QtApp,
  SfmlApp,
  BoostApp,
  TestProject
};

enum class BuildSystem {
  CMake,
  Meson,
  Bazel,
  XMake,
  Premake,
  Make,
  Ninja
};

enum class PackageManager {
  Vcpkg,
  Conan,
  None,
  Spack,
  Hunter,
  CPM,
  FetchContent
};

enum class TestFramework {
  GTest,
  Catch2,
  Doctest,
  Boost,
  None
};

enum class EditorConfig {
  VSCode,
  CLion,
  VS,
  Vim,
  Emacs,
  Sublime
};

enum class CiSystem {
  GitHub,
  GitLab,
  Travis,
  AppVeyor,
  AzureDevOps,
  CircleCI
};

enum class Language {
  English,
  Chinese,
  Spanish,
  Japanese,
  German,
  French
};

// Advanced configuration enums
enum class CppStandard {
  Cpp11,
  Cpp14,
  Cpp17,
  Cpp20,
  Cpp23,
  Latest
};

enum class ProjectStructure {
  Minimal,
  Standard,
  Advanced,
  Custom
};

enum class CompilerFlags {
  Debug,
  Release,
  RelWithDebInfo,
  MinSizeRel,
  Custom
};

// Git workflow and configuration enums
enum class GitWorkflow {
  None,
  GitFlow,
  GitHubFlow,
  GitLabFlow,
  Custom
};

enum class GitBranchStrategy {
  SingleBranch,
  FeatureBranches,
  GitFlow,
  Custom
};

enum class LicenseType {
  MIT,
  Apache2,
  GPL3,
  BSD3,
  BSD2,
  Unlicense,
  Custom,
  None
};

// Remote source types for project creation
enum class SourceType {
  Template,      // Use built-in template (default)
  GitRepository, // Clone from Git repository
  Archive,       // Extract from archive file
  LocalPath      // Copy from local directory
};

// Enum conversion utilities namespace
namespace cli_enums {

// Convert enums to strings
std::string_view to_string(TemplateType type);
std::string_view to_string(BuildSystem system);
std::string_view to_string(PackageManager manager);
std::string_view to_string(TestFramework framework);
std::string_view to_string(EditorConfig editor);
std::string_view to_string(CiSystem ci);
std::string_view to_string(Language lang);
std::string_view to_string(CppStandard standard);
std::string_view to_string(ProjectStructure structure);
std::string_view to_string(CompilerFlags flags);
std::string_view to_string(GitWorkflow workflow);
std::string_view to_string(GitBranchStrategy strategy);
std::string_view to_string(LicenseType license);
std::string_view to_string(SourceType source);

// Convert strings to enums
std::optional<TemplateType> to_template_type(std::string_view str);
std::optional<BuildSystem> to_build_system(std::string_view str);
std::optional<PackageManager> to_package_manager(std::string_view str);
std::optional<TestFramework> to_test_framework(std::string_view str);
std::optional<EditorConfig> to_editor_config(std::string_view str);
std::optional<CiSystem> to_ci_system(std::string_view str);
std::optional<Language> to_language(std::string_view str);
std::optional<CppStandard> to_cpp_standard(std::string_view str);
std::optional<ProjectStructure> to_project_structure(std::string_view str);
std::optional<CompilerFlags> to_compiler_flags(std::string_view str);
std::optional<GitWorkflow> to_git_workflow(std::string_view str);
std::optional<GitBranchStrategy> to_git_branch_strategy(std::string_view str);
std::optional<LicenseType> to_license_type(std::string_view str);
std::optional<SourceType> to_source_type(std::string_view str);

// Get all enum values as strings
std::vector<std::string_view> all_template_types();
std::vector<std::string_view> all_build_systems();
std::vector<std::string_view> all_package_managers();
std::vector<std::string_view> all_test_frameworks();
std::vector<std::string_view> all_editor_configs();
std::vector<std::string_view> all_ci_systems();
std::vector<std::string_view> all_languages();
std::vector<std::string_view> all_cpp_standards();
std::vector<std::string_view> all_project_structures();
std::vector<std::string_view> all_compiler_flags();
std::vector<std::string_view> all_git_workflows();
std::vector<std::string_view> all_git_branch_strategies();
std::vector<std::string_view> all_license_types();
std::vector<std::string_view> all_source_types();

} // namespace cli_enums

// fmt formatters for custom enums
template <>
struct fmt::formatter<TemplateType> : fmt::formatter<std::string_view> {
  auto format(TemplateType type, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(cli_enums::to_string(type), ctx);
  }
};

template <>
struct fmt::formatter<BuildSystem> : fmt::formatter<std::string_view> {
  auto format(BuildSystem system, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(cli_enums::to_string(system), ctx);
  }
};

template <>
struct fmt::formatter<PackageManager> : fmt::formatter<std::string_view> {
  auto format(PackageManager manager, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(cli_enums::to_string(manager), ctx);
  }
};

template <>
struct fmt::formatter<CiSystem> : fmt::formatter<std::string_view> {
  auto format(CiSystem ci, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(cli_enums::to_string(ci), ctx);
  }
};

template <>
struct fmt::formatter<SourceType> : fmt::formatter<std::string_view> {
  auto format(SourceType source, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(cli_enums::to_string(source), ctx);
  }
};
