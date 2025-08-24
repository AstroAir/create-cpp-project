#pragma once

#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace utils {

struct Dependency {
  std::string name;
  std::string version;
  std::string description;
  std::vector<std::string> features;
  bool required = true;
  bool headerOnly = false;
  std::string repository;
  std::string license;
};

struct PackageManagerConfig {
  std::string name;
  std::string configFile;
  std::string installCommand;
  std::string updateCommand;
  std::string searchCommand;
  bool supportsVersions = true;
  bool supportsFeatures = false;
};

class DependencyManager {
public:
  static DependencyManager& getInstance();
  
  // Package manager operations
  bool installDependencies(const CliOptions& options);
  bool updateDependencies(const CliOptions& options);
  bool searchPackage(const std::string& packageName);
  
  // Dependency resolution
  std::vector<Dependency> getRecommendedDependencies(TemplateType templateType);
  std::vector<Dependency> getTestingDependencies(TestFramework framework);
  std::vector<Dependency> getBuildDependencies(BuildSystem buildSystem);
  
  // Package manager specific operations
  bool setupVcpkg(const std::string& projectPath);
  bool setupConan(const std::string& projectPath);
  bool setupSpack(const std::string& projectPath);
  bool setupHunter(const std::string& projectPath);
  
  // Configuration management
  bool generateVcpkgJson(const std::string& projectPath, const std::vector<Dependency>& deps);
  bool generateConanfile(const std::string& projectPath, const std::vector<Dependency>& deps);
  bool generateCMakePresets(const std::string& projectPath, PackageManager pm);
  
  // Dependency validation
  bool validateDependencies(const std::vector<Dependency>& deps);
  bool checkDependencyCompatibility(const Dependency& dep1, const Dependency& dep2);
  
  // Interactive dependency selection
  std::vector<Dependency> selectDependenciesInteractively(const std::vector<Dependency>& available);
  
  // Utility functions
  std::string getLatestVersion(const std::string& packageName, PackageManager pm);
  std::vector<std::string> getAvailableVersions(const std::string& packageName, PackageManager pm);
  bool isPackageAvailable(const std::string& packageName, PackageManager pm);
  
private:
  DependencyManager() = default;
  
  // Internal helper methods
  PackageManagerConfig getPackageManagerConfig(PackageManager pm);
  std::string executeCommand(const std::string& command);
  bool writeConfigFile(const std::string& path, const std::string& content);
  
  // Dependency databases
  void initializeDependencyDatabase();
  std::unordered_map<std::string, Dependency> commonDependencies_;
  std::unordered_map<TemplateType, std::vector<std::string>> templateDependencies_;
  std::unordered_map<TestFramework, std::vector<std::string>> testDependencies_;
  
  // Package manager configurations
  std::unordered_map<PackageManager, PackageManagerConfig> packageManagerConfigs_;
};
