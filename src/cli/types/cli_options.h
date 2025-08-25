#pragma once

#include "cli_enums.h"
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Enhanced command line options structure
struct CliOptions {
  // Basic project information
  std::string projectName;
  std::string projectDescription;
  TemplateType templateType = TemplateType::Console;
  BuildSystem buildSystem = BuildSystem::CMake;
  PackageManager packageManager = PackageManager::Vcpkg;
  std::optional<std::string> networkLibrary; // For network projects: asio, boost, poco

  // Enhanced C++ configuration
  CppStandard cppStandard = CppStandard::Cpp17;
  ProjectStructure projectStructure = ProjectStructure::Standard;
  CompilerFlags compilerFlags = CompilerFlags::Release;
  std::vector<std::string> customCompilerFlags;

  // Testing configuration
  bool includeTests = false;
  TestFramework testFramework = TestFramework::GTest;

  // Documentation and code quality
  bool includeDocumentation = false;
  bool includeCodeStyleTools = false;

  // Development environment
  std::vector<EditorConfig> editorOptions;
  std::vector<CiSystem> ciOptions;

  // Git and version control
  bool initGit = true;
  GitWorkflow gitWorkflow = GitWorkflow::None;
  GitBranchStrategy gitBranchStrategy = GitBranchStrategy::SingleBranch;
  LicenseType licenseType = LicenseType::MIT;
  std::string gitRemoteUrl;
  std::string gitUserName;
  std::string gitUserEmail;
  bool createInitialCommit = true;
  bool setupGitHooks = false;
  std::vector<std::string> gitBranches;

  // Documentation-related options
  std::vector<std::string> docFormats{"markdown"};
  std::vector<std::string> docTypes{"readme", "api"};
  bool generateDoxygen = false;
  std::string doxygenTheme = "default";
  bool includeCodeExamples = true;
  bool generateChangelog = false;

  // Extended graphical framework options
  std::vector<std::string> guiFrameworks;
  std::vector<std::string> gameFrameworks;
  std::vector<std::string> graphicsLibraries;
  bool includeShaders = false;

  // CLI control flags
  bool showHelp = false;
  bool version = false;
  bool verbose = false;
  Language language = Language::English;
  std::filesystem::path customTemplatePath;

  // Profile and validation options
  std::string profileName;
  bool validateConfig = true;
  bool strictValidation = false;

  // Remote source options
  SourceType sourceType = SourceType::Template;   // Source type for project creation
  std::optional<std::string> gitRepositoryUrl;    // Git repository URL for cloning
  std::optional<std::string> archiveUrl;          // Archive URL for downloading
  std::optional<std::string> archiveFilePath;     // Local archive file path
  std::optional<std::string> gitBranch;           // Specific branch to checkout
  std::optional<std::string> gitTag;              // Specific tag to checkout
  std::optional<std::string> gitCommit;           // Specific commit to checkout
  bool useShallowClone = true;                    // Use shallow clone for faster downloads
  bool preserveGitHistory = false;                // Keep .git directory after cloning
  std::optional<std::string> gitUsername;         // Git authentication username
  std::optional<std::string> gitPassword;         // Git authentication password/token
  std::optional<std::string> sshKeyPath;          // SSH key path for Git authentication

  // Additional options for advanced use cases
  std::unordered_map<std::string, std::string> additionalOptions;
};
