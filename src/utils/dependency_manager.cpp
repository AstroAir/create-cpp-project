#include "dependency_manager.h"
#include "file_utils.h"
#include "terminal_utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fmt/format.h>

namespace utils {

DependencyManager& DependencyManager::getInstance() {
  static DependencyManager instance;
  return instance;
}

bool DependencyManager::installDependencies(const CliOptions& options) {
  TerminalUtils::showDotSpinner("Installing dependencies...", 2000);
  
  std::vector<Dependency> dependencies = getRecommendedDependencies(options.templateType);
  
  if (options.includeTests) {
    auto testDeps = getTestingDependencies(options.testFramework);
    dependencies.insert(dependencies.end(), testDeps.begin(), testDeps.end());
  }
  
  switch (options.packageManager) {
    case PackageManager::Vcpkg:
      return setupVcpkg(options.projectName) && 
             generateVcpkgJson(options.projectName, dependencies);
    case PackageManager::Conan:
      return setupConan(options.projectName) && 
             generateConanfile(options.projectName, dependencies);
    case PackageManager::Spack:
      return setupSpack(options.projectName);
    case PackageManager::Hunter:
      return setupHunter(options.projectName);
    default:
      TerminalUtils::showInfo("No package manager selected - dependencies will need to be installed manually");
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

} // namespace utils
