#include "cli_enums.h"
#include <unordered_map>

namespace cli_enums {

// Template type conversions
std::string_view to_string(TemplateType type) {
  static const std::unordered_map<TemplateType, std::string_view> map = {
      {TemplateType::Console, "console"},
      {TemplateType::Lib, "lib"},
      {TemplateType::HeaderOnlyLib, "header-only-lib"},
      {TemplateType::MultiExecutable, "multi-executable"},
      {TemplateType::Gui, "gui"},
      {TemplateType::Network, "network"},
      {TemplateType::Embedded, "embedded"},
      {TemplateType::WebService, "webservice"},
      {TemplateType::GameEngine, "gameengine"},
      {TemplateType::QtApp, "qt-app"},
      {TemplateType::SfmlApp, "sfml-app"},
      {TemplateType::BoostApp, "boost-app"},
      {TemplateType::TestProject, "test-project"}};
  return map.at(type);
}

std::optional<TemplateType> to_template_type(std::string_view str) {
  static const std::unordered_map<std::string_view, TemplateType> map = {
      {"console", TemplateType::Console},
      {"lib", TemplateType::Lib},
      {"header-only-lib", TemplateType::HeaderOnlyLib},
      {"multi-executable", TemplateType::MultiExecutable},
      {"gui", TemplateType::Gui},
      {"network", TemplateType::Network},
      {"embedded", TemplateType::Embedded},
      {"webservice", TemplateType::WebService},
      {"gameengine", TemplateType::GameEngine},
      {"qt-app", TemplateType::QtApp},
      {"sfml-app", TemplateType::SfmlApp},
      {"boost-app", TemplateType::BoostApp},
      {"test-project", TemplateType::TestProject}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<TemplateType>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_template_types() {
  return {"console", "lib", "header-only-lib", "multi-executable", "gui",
          "network", "embedded", "webservice", "gameengine", "qt-app",
          "sfml-app", "boost-app", "test-project"};
}

// Build system conversions
std::string_view to_string(BuildSystem system) {
  static const std::unordered_map<BuildSystem, std::string_view> map = {
      {BuildSystem::CMake, "cmake"},     {BuildSystem::Meson, "meson"},
      {BuildSystem::Bazel, "bazel"},     {BuildSystem::XMake, "xmake"},
      {BuildSystem::Premake, "premake"}, {BuildSystem::Make, "make"},
      {BuildSystem::Ninja, "ninja"}};
  return map.at(system);
}

std::optional<BuildSystem> to_build_system(std::string_view str) {
  static const std::unordered_map<std::string_view, BuildSystem> map = {
      {"cmake", BuildSystem::CMake},     {"meson", BuildSystem::Meson},
      {"bazel", BuildSystem::Bazel},     {"xmake", BuildSystem::XMake},
      {"premake", BuildSystem::Premake}, {"make", BuildSystem::Make},
      {"ninja", BuildSystem::Ninja}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<BuildSystem>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_build_systems() {
  return {"cmake", "meson", "bazel", "xmake", "premake", "make", "ninja"};
}

// Package manager conversions
std::string_view to_string(PackageManager manager) {
  static const std::unordered_map<PackageManager, std::string_view> map = {
      {PackageManager::Vcpkg, "vcpkg"},
      {PackageManager::Conan, "conan"},
      {PackageManager::None, "none"},
      {PackageManager::Spack, "spack"},
      {PackageManager::Hunter, "hunter"},
      {PackageManager::CPM, "cpm"},
      {PackageManager::FetchContent, "fetchcontent"}};
  return map.at(manager);
}

std::optional<PackageManager> to_package_manager(std::string_view str) {
  static const std::unordered_map<std::string_view, PackageManager> map = {
      {"vcpkg", PackageManager::Vcpkg}, {"conan", PackageManager::Conan},
      {"none", PackageManager::None}, {"spack", PackageManager::Spack},
      {"hunter", PackageManager::Hunter}, {"cpm", PackageManager::CPM},
      {"fetchcontent", PackageManager::FetchContent}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<PackageManager>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_package_managers() {
  return {"vcpkg", "conan", "none", "spack", "hunter", "cpm", "fetchcontent"};
}

// Test framework conversions
std::string_view to_string(TestFramework framework) {
  static const std::unordered_map<TestFramework, std::string_view> map = {
      {TestFramework::GTest, "gtest"},
      {TestFramework::Catch2, "catch2"},
      {TestFramework::Doctest, "doctest"},
      {TestFramework::Boost, "boost"},
      {TestFramework::None, "none"}};
  return map.at(framework);
}

std::optional<TestFramework> to_test_framework(std::string_view str) {
  static const std::unordered_map<std::string_view, TestFramework> map = {
      {"gtest", TestFramework::GTest}, {"catch2", TestFramework::Catch2},
      {"doctest", TestFramework::Doctest}, {"boost", TestFramework::Boost},
      {"none", TestFramework::None}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<TestFramework>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_test_frameworks() {
  return {"gtest", "catch2", "doctest", "boost", "none"};
}

// Editor config conversions
std::string_view to_string(EditorConfig editor) {
  static const std::unordered_map<EditorConfig, std::string_view> map = {
      {EditorConfig::VSCode, "vscode"}, {EditorConfig::CLion, "clion"},
      {EditorConfig::VS, "vs"},         {EditorConfig::Vim, "vim"},
      {EditorConfig::Emacs, "emacs"},   {EditorConfig::Sublime, "sublime"}};
  return map.at(editor);
}

std::optional<EditorConfig> to_editor_config(std::string_view str) {
  static const std::unordered_map<std::string_view, EditorConfig> map = {
      {"vscode", EditorConfig::VSCode}, {"clion", EditorConfig::CLion},
      {"vs", EditorConfig::VS}, {"vim", EditorConfig::Vim},
      {"emacs", EditorConfig::Emacs}, {"sublime", EditorConfig::Sublime}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<EditorConfig>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_editor_configs() {
  return {"vscode", "clion", "vs", "vim", "emacs", "sublime"};
}

// CI system conversions
std::string_view to_string(CiSystem ci) {
  static const std::unordered_map<CiSystem, std::string_view> map = {
      {CiSystem::GitHub, "github"},     {CiSystem::GitLab, "gitlab"},
      {CiSystem::Travis, "travis"},     {CiSystem::AppVeyor, "appveyor"},
      {CiSystem::AzureDevOps, "azure"}, {CiSystem::CircleCI, "circleci"}};
  return map.at(ci);
}

std::optional<CiSystem> to_ci_system(std::string_view str) {
  static const std::unordered_map<std::string_view, CiSystem> map = {
      {"github", CiSystem::GitHub}, {"gitlab", CiSystem::GitLab},
      {"travis", CiSystem::Travis}, {"appveyor", CiSystem::AppVeyor},
      {"azure", CiSystem::AzureDevOps}, {"circleci", CiSystem::CircleCI}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<CiSystem>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_ci_systems() {
  return {"github", "gitlab", "travis", "appveyor", "azure", "circleci"};
}

// Language conversions
std::string_view to_string(Language lang) {
  static const std::unordered_map<Language, std::string_view> map = {
      {Language::English, "en"}, {Language::Chinese, "zh"},
      {Language::Spanish, "es"}, {Language::Japanese, "jp"},
      {Language::German, "de"},  {Language::French, "fr"}};
  return map.at(lang);
}

std::optional<Language> to_language(std::string_view str) {
  static const std::unordered_map<std::string_view, Language> map = {
      {"en", Language::English}, {"zh", Language::Chinese},
      {"es", Language::Spanish}, {"jp", Language::Japanese},
      {"de", Language::German}, {"fr", Language::French}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<Language>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_languages() {
  return {"en", "zh", "es", "jp", "de", "fr"};
}

// C++ standard conversions
std::string_view to_string(CppStandard standard) {
  static const std::unordered_map<CppStandard, std::string_view> map = {
      {CppStandard::Cpp11, "c++11"},
      {CppStandard::Cpp14, "c++14"},
      {CppStandard::Cpp17, "c++17"},
      {CppStandard::Cpp20, "c++20"},
      {CppStandard::Cpp23, "c++23"},
      {CppStandard::Latest, "latest"}};
  return map.at(standard);
}

std::optional<CppStandard> to_cpp_standard(std::string_view str) {
  static const std::unordered_map<std::string_view, CppStandard> map = {
      {"c++11", CppStandard::Cpp11}, {"11", CppStandard::Cpp11},
      {"c++14", CppStandard::Cpp14}, {"14", CppStandard::Cpp14},
      {"c++17", CppStandard::Cpp17}, {"17", CppStandard::Cpp17},
      {"c++20", CppStandard::Cpp20}, {"20", CppStandard::Cpp20},
      {"c++23", CppStandard::Cpp23}, {"23", CppStandard::Cpp23},
      {"latest", CppStandard::Latest}};
  auto it = map.find(str);
  return it != map.end() ? std::optional<CppStandard>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_cpp_standards() {
  return {"c++11", "c++14", "c++17", "c++20", "c++23", "latest"};
}

// Project structure conversions
std::string_view to_string(ProjectStructure structure) {
  static const std::unordered_map<ProjectStructure, std::string_view> map = {
      {ProjectStructure::Minimal, "minimal"},
      {ProjectStructure::Standard, "standard"},
      {ProjectStructure::Advanced, "advanced"},
      {ProjectStructure::Custom, "custom"}};
  return map.at(structure);
}

std::optional<ProjectStructure> to_project_structure(std::string_view str) {
  static const std::unordered_map<std::string_view, ProjectStructure> map = {
      {"minimal", ProjectStructure::Minimal},
      {"standard", ProjectStructure::Standard},
      {"advanced", ProjectStructure::Advanced},
      {"custom", ProjectStructure::Custom}};
  auto it = map.find(str);
  return it != map.end() ? std::optional<ProjectStructure>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_project_structures() {
  return {"minimal", "standard", "advanced", "custom"};
}

// Compiler flags conversions
std::string_view to_string(CompilerFlags flags) {
  static const std::unordered_map<CompilerFlags, std::string_view> map = {
      {CompilerFlags::Debug, "debug"},
      {CompilerFlags::Release, "release"},
      {CompilerFlags::RelWithDebInfo, "relwithdebinfo"},
      {CompilerFlags::MinSizeRel, "minsizerel"},
      {CompilerFlags::Custom, "custom"}};
  return map.at(flags);
}

std::optional<CompilerFlags> to_compiler_flags(std::string_view str) {
  static const std::unordered_map<std::string_view, CompilerFlags> map = {
      {"debug", CompilerFlags::Debug},
      {"release", CompilerFlags::Release},
      {"relwithdebinfo", CompilerFlags::RelWithDebInfo},
      {"minsizerel", CompilerFlags::MinSizeRel},
      {"custom", CompilerFlags::Custom}};
  auto it = map.find(str);
  return it != map.end() ? std::optional<CompilerFlags>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_compiler_flags() {
  return {"debug", "release", "relwithdebinfo", "minsizerel", "custom"};
}

// Git workflow conversions
std::string_view to_string(GitWorkflow workflow) {
  static const std::unordered_map<GitWorkflow, std::string_view> map = {
      {GitWorkflow::None, "none"}, {GitWorkflow::GitFlow, "gitflow"},
      {GitWorkflow::GitHubFlow, "github-flow"}, {GitWorkflow::GitLabFlow, "gitlab-flow"},
      {GitWorkflow::Custom, "custom"}};
  return map.at(workflow);
}

std::optional<GitWorkflow> to_git_workflow(std::string_view str) {
  static const std::unordered_map<std::string_view, GitWorkflow> map = {
      {"none", GitWorkflow::None}, {"gitflow", GitWorkflow::GitFlow},
      {"github-flow", GitWorkflow::GitHubFlow}, {"gitlab-flow", GitWorkflow::GitLabFlow},
      {"custom", GitWorkflow::Custom}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<GitWorkflow>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_git_workflows() {
  return {"none", "gitflow", "github-flow", "gitlab-flow", "custom"};
}

// Git branch strategy conversions
std::string_view to_string(GitBranchStrategy strategy) {
  static const std::unordered_map<GitBranchStrategy, std::string_view> map = {
      {GitBranchStrategy::SingleBranch, "single"}, {GitBranchStrategy::FeatureBranches, "feature"},
      {GitBranchStrategy::GitFlow, "gitflow"}, {GitBranchStrategy::Custom, "custom"}};
  return map.at(strategy);
}

std::optional<GitBranchStrategy> to_git_branch_strategy(std::string_view str) {
  static const std::unordered_map<std::string_view, GitBranchStrategy> map = {
      {"single", GitBranchStrategy::SingleBranch}, {"feature", GitBranchStrategy::FeatureBranches},
      {"gitflow", GitBranchStrategy::GitFlow}, {"custom", GitBranchStrategy::Custom}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<GitBranchStrategy>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_git_branch_strategies() {
  return {"single", "feature", "gitflow", "custom"};
}

// License type conversions
std::string_view to_string(LicenseType license) {
  static const std::unordered_map<LicenseType, std::string_view> map = {
      {LicenseType::MIT, "mit"}, {LicenseType::Apache2, "apache2"},
      {LicenseType::GPL3, "gpl3"}, {LicenseType::BSD3, "bsd3"},
      {LicenseType::BSD2, "bsd2"}, {LicenseType::Unlicense, "unlicense"},
      {LicenseType::Custom, "custom"}, {LicenseType::None, "none"}};
  return map.at(license);
}

std::optional<LicenseType> to_license_type(std::string_view str) {
  static const std::unordered_map<std::string_view, LicenseType> map = {
      {"mit", LicenseType::MIT}, {"apache2", LicenseType::Apache2},
      {"gpl3", LicenseType::GPL3}, {"bsd3", LicenseType::BSD3},
      {"bsd2", LicenseType::BSD2}, {"unlicense", LicenseType::Unlicense},
      {"custom", LicenseType::Custom}, {"none", LicenseType::None}};

  auto it = map.find(str);
  return it != map.end() ? std::optional<LicenseType>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_license_types() {
  return {"mit", "apache2", "gpl3", "bsd3", "bsd2", "unlicense", "custom", "none"};
}

} // namespace cli_enums
