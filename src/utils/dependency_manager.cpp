#include "dependency_manager.h"
#include "file_utils.h"
#include "terminal_utils.h"
#include <sstream>
#include <algorithm>
#include <fmt/format.h>

namespace utils {

DependencyManager& DependencyManager::getInstance() {
  static DependencyManager instance;
  return instance;
}

bool DependencyManager::installDependencies(const CliOptions& options) {
  TerminalUtils::showNpmStyleProgress("Installing dependencies", 0, "Resolving dependencies");

  std::vector<Dependency> requestedDeps = getRecommendedDependencies(options.templateType);

  if (options.includeTests) {
    auto testDeps = getTestingDependencies(options.testFramework);
    requestedDeps.insert(requestedDeps.end(), testDeps.begin(), testDeps.end());
  }

  TerminalUtils::showNpmStyleProgress("Installing dependencies", 20, "Validating dependencies");

  // Enhanced dependency resolution
  DependencyResolution resolution = resolveDependencies(requestedDeps, options);

  if (!resolution.success) {
    TerminalUtils::showNpmStyleError("Dependency resolution failed", "Conflicts detected");
    for (const auto& conflict : resolution.conflicts) {
      TerminalUtils::showNpmStyleError("Conflict", conflict);
    }
    return false;
  }

  if (!resolution.warnings.empty()) {
    for (const auto& warning : resolution.warnings) {
      TerminalUtils::showNpmStyleWarning("Dependency warning", warning);
    }
  }

  TerminalUtils::showNpmStyleProgress("Installing dependencies", 40, "Setting up package manager");

  switch (options.packageManager) {
    case PackageManager::Vcpkg:
      TerminalUtils::showNpmStyleProgress("Installing dependencies", 60, "Configuring vcpkg");
      return setupVcpkg(options.projectName) &&
             generateVcpkgJson(options.projectName, resolution.resolved);
    case PackageManager::Conan:
      TerminalUtils::showNpmStyleProgress("Installing dependencies", 60, "Configuring Conan");
      return setupConan(options.projectName) &&
             generateConanfile(options.projectName, resolution.resolved);
    case PackageManager::Spack:
      TerminalUtils::showNpmStyleProgress("Installing dependencies", 60, "Configuring Spack");
      return setupSpack(options.projectName);
    case PackageManager::Hunter:
      TerminalUtils::showNpmStyleProgress("Installing dependencies", 60, "Configuring Hunter");
      return setupHunter(options.projectName);
    case PackageManager::CPM:
      TerminalUtils::showNpmStyleProgress("Installing dependencies", 60, "Configuring CPM");
      return setupCpm(options.projectName);
    case PackageManager::FetchContent:
      TerminalUtils::showNpmStyleProgress("Installing dependencies", 60, "Configuring FetchContent");
      return setupFetchContent(options.projectName);
    default:
      TerminalUtils::showNpmStyleWarning("No package manager selected", "Dependencies will need to be installed manually");
      return true;
  }
}

::std::vector<Dependency> DependencyManager::getRecommendedDependencies(TemplateType templateType) {
  ::std::vector<Dependency> deps;

  // Common dependencies for all projects
  deps.push_back({"fmt", "9.1.0", "Modern formatting library", {}, true, true,
                  "https://github.com/fmtlib/fmt", "MIT"});
  deps.push_back({"spdlog", "1.11.0", "Fast C++ logging library", {}, true, true,
                  "https://github.com/gabime/spdlog", "MIT"});

  switch (templateType) {
    case TemplateType::Console:
      deps.push_back({"cli11", "2.3.2", "Command line parser", {}, false, true,
                      "https://github.com/CLIUtils/CLI11", "BSD-3-Clause"});
      break;

    case TemplateType::Lib:
      deps.push_back({"benchmark", "1.7.1", "Microbenchmark library", {}, false, false,
                      "https://github.com/google/benchmark", "Apache-2.0"});
      break;

    case TemplateType::Network:
      deps.push_back({"asio", "1.24.0", "Asynchronous I/O library", {}, true, true,
                      "https://github.com/chriskohlhoff/asio", "BSL-1.0"});
      deps.push_back({"nlohmann-json", "3.11.2", "JSON library", {}, true, true,
                      "https://github.com/nlohmann/json", "MIT"});
      break;

    case TemplateType::WebService:
      deps.push_back({"httplib", "0.14.0", "HTTP/HTTPS server and client library", {}, true, true,
                      "https://github.com/yhirose/cpp-httplib", "MIT"});
      deps.push_back({"nlohmann-json", "3.11.2", "JSON library", {}, true, true,
                      "https://github.com/nlohmann/json", "MIT"});
      deps.push_back({"sqlite3", "3.41.2", "SQLite database", {}, false, false,
                      "https://www.sqlite.org/", "Public Domain"});
      break;

    case TemplateType::Gui:
      deps.push_back({"qt5", "5.15.8", "Cross-platform GUI toolkit", {"widgets", "network"}, true, false,
                      "https://www.qt.io/", "LGPL-3.0"});
      break;

    case TemplateType::Embedded:
      // Embedded projects typically use vendor-specific libraries
      deps.push_back({"freertos", "10.4.6", "Real-time operating system", {}, false, false,
                      "https://www.freertos.org/", "MIT"});
      break;

    case TemplateType::GameEngine:
      deps.push_back({"glfw3", "3.3.8", "OpenGL framework", {}, true, false,
                      "https://www.glfw.org/", "Zlib"});
      deps.push_back({"glm", "0.9.9.8", "OpenGL Mathematics", {}, true, true,
                      "https://github.com/g-truc/glm", "MIT"});
      break;

    case TemplateType::HeaderOnlyLib:
      // Header-only libraries typically have minimal dependencies
      deps.push_back({"catch2", "3.3.2", "Testing framework", {}, false, true,
                      "https://github.com/catchorg/Catch2", "BSL-1.0"});
      break;

    case TemplateType::MultiExecutable:
      // Multi-executable projects might need shared utilities
      deps.push_back({"cli11", "2.3.2", "Command line parser", {}, false, true,
                      "https://github.com/CLIUtils/CLI11", "BSD-3-Clause"});
      deps.push_back({"toml11", "3.7.1", "TOML configuration parser", {}, false, true,
                      "https://github.com/ToruNiina/toml11", "MIT"});
      break;

    case TemplateType::QtApp:
      deps.push_back({"qt6", "6.5.0", "Cross-platform GUI toolkit", {"widgets", "network", "core"}, true, false,
                      "https://www.qt.io/", "LGPL-3.0"});
      break;

    case TemplateType::SfmlApp:
      deps.push_back({"sfml", "2.6.0", "Simple and Fast Multimedia Library", {"graphics", "window", "system"}, true, false,
                      "https://www.sfml-dev.org/", "Zlib"});
      break;

    case TemplateType::BoostApp:
      deps.push_back({"boost", "1.82.0", "Boost C++ Libraries", {"system", "filesystem", "thread"}, true, false,
                      "https://www.boost.org/", "BSL-1.0"});
      break;

    case TemplateType::TestProject:
      deps.push_back({"gtest", "1.13.0", "Google Test framework", {}, true, false,
                      "https://github.com/google/googletest", "BSD-3-Clause"});
      deps.push_back({"gmock", "1.13.0", "Google Mock framework", {}, false, false,
                      "https://github.com/google/googletest", "BSD-3-Clause"});
      break;
  }

  return deps;
}

::std::vector<Dependency> DependencyManager::getTestingDependencies(TestFramework framework) {
  ::std::vector<Dependency> deps;

  switch (framework) {
    case TestFramework::GTest:
      deps.push_back({"gtest", "1.13.0", "Google Test framework", {"gmock"}, true, false,
                      "https://github.com/google/googletest", "BSD-3-Clause"});
      break;
    case TestFramework::Catch2:
      deps.push_back({"catch2", "3.3.2", "Modern C++ test framework", {}, true, true,
                      "https://github.com/catchorg/Catch2", "BSL-1.0"});
      break;
    case TestFramework::Doctest:
      deps.push_back({"doctest", "2.4.9", "Lightweight C++ test framework", {}, true, true,
                      "https://github.com/doctest/doctest", "MIT"});
      break;
    case TestFramework::Boost:
      deps.push_back({"boost-test", "1.81.0", "Boost Test library", {}, true, false,
                      "https://www.boost.org/", "BSL-1.0"});
      break;
    default:
      break;
  }

  return deps;
}

bool DependencyManager::setupVcpkg(const ::std::string& projectPath) {
  TerminalUtils::showInfo("Setting up vcpkg integration...");

  // Check if vcpkg is available
  std::string vcpkgCheck = executeCommand("vcpkg version");
  if (vcpkgCheck.empty()) {
    TerminalUtils::showWarning("vcpkg not found in PATH. Please install vcpkg first.");
    TerminalUtils::showInfo("Visit: https://github.com/Microsoft/vcpkg");
    return false;
  }

  TerminalUtils::showSuccess("vcpkg found and ready to use");
  return true;
}

bool DependencyManager::setupConan(const ::std::string& projectPath) {
  TerminalUtils::showInfo("Setting up Conan integration...");

  // Check if Conan is available
  std::string conanCheck = executeCommand("conan --version");
  if (conanCheck.empty()) {
    TerminalUtils::showWarning("Conan not found in PATH. Please install Conan first.");
    TerminalUtils::showInfo("Run: pip install conan");
    return false;
  }

  // Initialize Conan profile if needed
  executeCommand("conan profile detect --force");

  TerminalUtils::showSuccess("Conan found and configured");
  return true;
}

bool DependencyManager::generateVcpkgJson(const ::std::string& projectPath,
                                          const ::std::vector<Dependency>& deps) {
  std::ostringstream json;
  json << "{\n";
  json << "  \"name\": \"" << projectPath << "\",\n";
  json << "  \"version\": \"1.0.0\",\n";
  json << "  \"dependencies\": [\n";

  for (size_t i = 0; i < deps.size(); ++i) {
    const auto& dep = deps[i];
    if (i > 0) json << ",\n";

    if (dep.features.empty()) {
      json << "    \"" << dep.name << "\"";
    } else {
      json << "    {\n";
      json << "      \"name\": \"" << dep.name << "\",\n";
      json << "      \"features\": [";
      for (size_t j = 0; j < dep.features.size(); ++j) {
        if (j > 0) json << ", ";
        json << "\"" << dep.features[j] << "\"";
      }
      json << "]\n";
      json << "    }";
    }
  }

  json << "\n  ]\n";
  json << "}\n";

  std::string vcpkgPath = FileUtils::combinePath(projectPath, "vcpkg.json");
  return writeConfigFile(vcpkgPath, json.str());
}

bool DependencyManager::generateConanfile(const ::std::string& projectPath,
                                          const ::std::vector<Dependency>& deps) {
  std::ostringstream conanfile;
  conanfile << "[requires]\n";

  for (const auto& dep : deps) {
    conanfile << dep.name << "/" << dep.version << "\n";
  }

  conanfile << "\n[generators]\n";
  conanfile << "CMakeDeps\n";
  conanfile << "CMakeToolchain\n";

  conanfile << "\n[options]\n";
  for (const auto& dep : deps) {
    if (!dep.headerOnly) {
      conanfile << dep.name << ":shared=False\n";
    }
  }

  std::string conanPath = FileUtils::combinePath(projectPath, "conanfile.txt");
  return writeConfigFile(conanPath, conanfile.str());
}

::std::string DependencyManager::executeCommand(const ::std::string& command) {
  // Simple command execution - in a real implementation, this would be more robust
  return ""; // Placeholder
}

bool DependencyManager::writeConfigFile(const ::std::string& path, const ::std::string& content) {
  return FileUtils::writeToFile(path, content);
}

// Enhanced dependency resolution
DependencyResolution DependencyManager::resolveDependencies(const std::vector<Dependency>& requested, const CliOptions& options) {
  DependencyResolution result;
  result.success = true;

  // Start with requested dependencies
  result.resolved = requested;

  // Validate each dependency
  for (const auto& dep : requested) {
    if (!validateDependencyName(dep.name)) {
      result.conflicts.push_back("Invalid dependency name: " + dep.name);
      result.success = false;
      continue;
    }

    if (!validateVersionString(dep.version)) {
      result.warnings.push_back("Invalid version for " + dep.name + ": " + dep.version);
    }

    // Check platform compatibility
    if (!checkPlatformCompatibility(dep, "current")) {
      result.warnings.push_back("Platform compatibility issues for " + dep.name);
    }

    // Check for conflicts with other dependencies
    for (const auto& other : requested) {
      if (dep.name != other.name && !checkDependencyCompatibility(dep, other)) {
        result.conflicts.push_back("Conflict between " + dep.name + " and " + other.name);
        result.success = false;
      }
    }
  }

  return result;
}

// New package manager setup methods
bool DependencyManager::setupCpm(const std::string& projectPath) {
  TerminalUtils::showNpmStyleProgress("Setting up CPM", 50, "Creating CPM configuration");

  std::string cpmContent = R"(# CPM.cmake configuration
include(cmake/CPM.cmake)

# Add dependencies here
# CPMAddPackage("gh:fmtlib/fmt#9.1.0")
# CPMAddPackage("gh:gabime/spdlog#v1.11.0")
)";

  return FileUtils::writeToFile(projectPath + "/cmake/CPM.cmake.in", cpmContent);
}

bool DependencyManager::setupFetchContent(const std::string& projectPath) {
  TerminalUtils::showNpmStyleProgress("Setting up FetchContent", 50, "Creating FetchContent configuration");

  std::string fetchContentCMake = R"(# FetchContent configuration
include(FetchContent)

# Example dependencies
# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG        9.1.0
# )
# FetchContent_MakeAvailable(fmt)
)";

  return FileUtils::writeToFile(projectPath + "/cmake/FetchContent.cmake", fetchContentCMake);
}

// Enhanced validation methods
bool DependencyManager::validateDependencyName(const std::string& name) {
  if (name.empty() || name.length() > 100) {
    return false;
  }

  // Check for valid characters (alphanumeric, hyphens, underscores, dots)
  for (char c : name) {
    if (!std::isalnum(c) && c != '-' && c != '_' && c != '.') {
      return false;
    }
  }

  return true;
}

bool DependencyManager::validateVersionString(const std::string& version) {
  if (version.empty()) {
    return false;
  }

  // Basic version validation - would need regex include for full implementation
  return !version.empty() && version.find_first_not_of("0123456789.-+abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos;
}

bool DependencyManager::checkPlatformCompatibility(const Dependency& dep, const std::string& platform) {
  // If no platform restrictions specified, assume compatible
  if (dep.platforms.empty()) {
    return true;
  }

  // Check if current platform is in the supported list
  return std::find(dep.platforms.begin(), dep.platforms.end(), platform) != dep.platforms.end();
}

} // namespace utils
