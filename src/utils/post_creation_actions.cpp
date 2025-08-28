#include "post_creation_actions.h"
#include "dependency_manager.h"
#include "terminal_utils.h"
#include "file_utils.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <fmt/format.h>

namespace utils {

PostCreationActions& PostCreationActions::getInstance() {
  static PostCreationActions instance;
  return instance;
}

bool PostCreationActions::executeAll(const CliOptions& options) {
  results_.clear();

  TerminalUtils::showBrandedHeader("Post-Creation Setup", "Finalizing your project...");

  // Define the sequence of actions
  std::vector<std::pair<std::string, std::function<ActionStatus()>>> actions = {
    {"validate", [&]() { return validateProjectStructure(options); }},
    {"dependencies", [&]() { return installDependencies(options); }},
    {"build", [&]() { return runInitialBuild(options); }},
    {"tests", [&]() { return runInitialTests(options); }},
    {"dev-env", [&]() { return setupDevelopmentEnvironment(options); }},
    {"docs", [&]() { return generateDocumentation(options); }},
    {"ci", [&]() { return setupContinuousIntegration(options); }},
    {"git", [&]() { return createInitialCommit(options); }}
  };

  int totalActions = actions.size();
  int currentAction = 0;

  for (const auto& [name, action] : actions) {
    if (!isActionEnabled(name)) {
      continue;
    }

    currentAction++;

    if (progressCallback_) {
      progressCallback_(fmt::format("Executing {}", name),
                       (currentAction * 100) / totalActions);
    }

    TerminalUtils::showStepIndicator(currentAction, totalActions,
                                   fmt::format("Running {}", name));

    ActionStatus status = action();
    results_.push_back(status);

    // Show result
    switch (status.result) {
      case ActionResult::Success:
        TerminalUtils::showSuccess(fmt::format("✅ {} completed successfully", status.name));
        break;
      case ActionResult::Warning:
        TerminalUtils::showWarning(fmt::format("⚠️  {} completed with warnings: {}",
                                              status.name, status.message));
        break;
      case ActionResult::Error:
        TerminalUtils::showError(fmt::format("❌ {} failed: {}",
                                            status.name, status.message));
        if (!skipOptional_) {
          return false;
        }
        break;
      case ActionResult::Skipped:
        TerminalUtils::showInfo(fmt::format("⏭️  {} skipped: {}",
                                           status.name, status.message));
        break;
    }
  }

  printSummary();
  return !hasErrors();
}

ActionStatus PostCreationActions::validateProjectStructure(const CliOptions& options) {
  return executeAction("Project Validation",
                      "Validating project structure and files",
                      [&]() {
    std::string projectPath = options.projectName;

    // Check essential directories
    std::vector<std::string> requiredDirs = {"src", "include"};
    for (const auto& dir : requiredDirs) {
      if (!validateDirectory(::utils::FileUtils::combinePath(projectPath, dir))) {
        return false;
      }
    }

    // Check essential files
    std::vector<std::string> requiredFiles = {"README.md"};

    // Add build system file
    switch (options.buildSystem) {
      case BuildSystem::CMake:
        requiredFiles.push_back("CMakeLists.txt");
        break;
      case BuildSystem::Meson:
        requiredFiles.push_back("meson.build");
        break;
      case BuildSystem::Bazel:
        requiredFiles.push_back("BUILD");
        break;
      default:
        requiredFiles.push_back("Makefile");
        break;
    }

    for (const auto& file : requiredFiles) {
      if (!validateFile(::utils::FileUtils::combinePath(projectPath, file))) {
        return false;
      }
    }

    return true;
  });
}

ActionStatus PostCreationActions::installDependencies(const CliOptions& options) {
  return executeAction("Dependency Installation",
                      "Installing project dependencies",
                      [&]() {
    if (options.packageManager == PackageManager::None) {
      return true; // Skip if no package manager
    }

    utils::DependencyManager& depManager = utils::DependencyManager::getInstance();
    return depManager.installDependencies(options);
  });
}

ActionStatus PostCreationActions::runInitialBuild(const CliOptions& options) {
  return executeAction("Initial Build",
                      "Building the project for the first time",
                      [&]() {
    std::string projectPath = options.projectName;

    switch (options.buildSystem) {
      case BuildSystem::CMake:
        return buildWithCMake(projectPath);
      case BuildSystem::Meson:
        return buildWithMeson(projectPath);
      case BuildSystem::Bazel:
        return buildWithBazel(projectPath);
      case BuildSystem::Make:
        return buildWithMake(projectPath);
      default:
        return buildWithCMake(projectPath);
    }
  });
}

ActionStatus PostCreationActions::runInitialTests(const CliOptions& options) {
  return executeAction("Initial Tests",
                      "Running initial test suite",
                      [&]() {
    if (!options.includeTests) {
      return true; // Skip if no tests
    }

    std::string projectPath = options.projectName;

    // Try CTest first (works with CMake)
    if (options.buildSystem == BuildSystem::CMake) {
      return runCTests(projectPath);
    }

    // Fallback to custom test runner
    return runCustomTests(projectPath);
  });
}

ActionStatus PostCreationActions::setupDevelopmentEnvironment(const CliOptions& options) {
  return executeAction("Development Environment",
                      "Setting up IDE configurations",
                      [&]() {
    std::string projectPath = options.projectName;
    bool success = true;

    for (const auto& editor : options.editorOptions) {
      switch (editor) {
        case EditorConfig::VSCode:
          success &= setupVSCode(projectPath);
          break;
        case EditorConfig::CLion:
          success &= setupCLion(projectPath);
          break;
        case EditorConfig::VS:
          success &= setupVS(projectPath);
          break;
        default:
          break;
      }
    }

    return success;
  });
}

ActionStatus PostCreationActions::createInitialCommit(const CliOptions& options) {
  return executeAction("Initial Git Commit",
                      "Creating initial Git commit",
                      [&]() {
    if (!options.initGit) {
      return true; // Skip if Git not requested
    }

    std::string projectPath = options.projectName;

    // Git should already be initialized by the template
    // Just need to add and commit files
    return runCommand("git add .", projectPath) &&
           runCommand("git commit -m \"Initial commit from cpp-scaffold\"", projectPath);
  });
}

ActionStatus PostCreationActions::executeAction(const std::string& name,
                                               const std::string& description,
                                               std::function<bool()> action) {
  ActionStatus status;
  status.name = name;
  status.description = description;

  auto start = std::chrono::high_resolution_clock::now();

  try {
    bool success = action();
    status.result = success ? ActionResult::Success : ActionResult::Error;
    if (!success) {
      status.message = "Action failed";
    }
  } catch (const std::exception& e) {
    status.result = ActionResult::Error;
    status.message = e.what();
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  status.duration = duration.count() / 1000.0;

  return status;
}

bool PostCreationActions::runCommand(const std::string& command, const std::string& workingDir) {
  // Simple command execution - in a real implementation, this would be more robust
  std::string fullCommand = command;
  if (!workingDir.empty()) {
    fullCommand = fmt::format("cd {} && {}", workingDir, command);
  }

  if (verbose_) {
    TerminalUtils::showInfo(fmt::format("Executing: {}", fullCommand));
  }

  int result = system(fullCommand.c_str());
  return result == 0;
}

bool PostCreationActions::validateFile(const std::string& path, const std::string& expectedContent) {
  (void)expectedContent; // TODO: Implement content validation
  return ::utils::FileUtils::fileExists(path);
}

bool PostCreationActions::validateDirectory(const std::string& path) {
  return ::utils::FileUtils::directoryExists(path);
}

bool PostCreationActions::buildWithCMake(const std::string& projectPath) {
  return runCommand("mkdir -p build", projectPath) &&
         runCommand("cd build && cmake ..", projectPath) &&
         runCommand("cd build && make -j$(nproc)", projectPath);
}

bool PostCreationActions::isActionEnabled(const std::string& actionName) const {
  auto it = enabledActions_.find(actionName);
  return it == enabledActions_.end() || it->second; // Default to enabled
}

void PostCreationActions::printSummary() const {
  std::cout << "\n";
  TerminalUtils::showBrandedHeader("Setup Summary", "Project creation completed!");

  int successful = 0, warnings = 0, errors = 0, skipped = 0;

  for (const auto& result : results_) {
    switch (result.result) {
      case ActionResult::Success: successful++; break;
      case ActionResult::Warning: warnings++; break;
      case ActionResult::Error: errors++; break;
      case ActionResult::Skipped: skipped++; break;
    }
  }

  std::cout << fmt::format("✅ Successful: {}\n", successful);
  if (warnings > 0) std::cout << fmt::format("⚠️  Warnings: {}\n", warnings);
  if (errors > 0) std::cout << fmt::format("❌ Errors: {}\n", errors);
  if (skipped > 0) std::cout << fmt::format("⏭️  Skipped: {}\n", skipped);

  std::cout << "\n";
}

bool PostCreationActions::hasErrors() const {
  return std::any_of(results_.begin(), results_.end(),
                    [](const ActionStatus& status) {
                      return status.result == ActionResult::Error;
                    });
}

} // namespace utils
