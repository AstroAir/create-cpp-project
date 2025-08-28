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
  std::string category;
  std::vector<std::string> platforms;
  std::vector<std::string> compilers;
  std::string minCppStandard;
  std::vector<std::string> conflicts;
  std::vector<std::string> alternatives;
  std::unordered_map<std::string, std::string> metadata;

  // Version constraints
  std::string minVersion;
  std::string maxVersion;
  bool allowPrerelease = false;

  // Build configuration
  std::vector<std::string> buildOptions;
  std::unordered_map<std::string, std::string> cmakeOptions;

  // Installation info
  std::string installationNotes;
  std::vector<std::string> postInstallSteps;
};

struct PackageManagerConfig {
  std::string name;
  std::string configFile;
  std::string installCommand;
  std::string updateCommand;
  std::string searchCommand;
  std::string listCommand;
  std::string removeCommand;
  std::string infoCommand;
  bool supportsVersions = true;
  bool supportsFeatures = false;
  bool supportsProfiles = false;
  bool supportsLockfiles = false;
  std::vector<std::string> supportedPlatforms;
  std::string executable;
  std::string versionFlag;
  std::unordered_map<std::string, std::string> environmentVars;
};

// Version constraint types
enum class VersionConstraintType {
  Exact,      // ==1.0.0
  Minimum,    // >=1.0.0
  Maximum,    // <=1.0.0
  Range,      // >=1.0.0,<2.0.0
  Compatible, // ~1.0.0 (compatible with 1.x.x)
  Caret,      // ^1.0.0 (compatible with 1.x.x but not 2.x.x)
  Latest,     // latest available
  Any         // any version
};

struct VersionConstraint {
  VersionConstraintType type;
  std::string version;
  std::string maxVersion; // for range constraints

  std::string toString() const;
  bool satisfies(const std::string& candidateVersion) const;
};

// Dependency resolution result
struct DependencyResolution {
  std::vector<Dependency> resolved;
  std::vector<std::string> conflicts;
  std::vector<std::string> warnings;
  std::vector<std::string> suggestions;
  bool success = true;
};

// Package information from registry
struct PackageInfo {
  std::string name;
  std::string latestVersion;
  std::vector<std::string> availableVersions;
  std::string description;
  std::string homepage;
  std::string repository;
  std::string license;
  std::vector<std::string> keywords;
  std::unordered_map<std::string, std::string> metadata;
  std::vector<Dependency> dependencies;
  bool isDeprecated = false;
  std::string deprecationMessage;
};

class DependencyManager {
public:
  static DependencyManager& getInstance();

  // Package manager operations
  bool installDependencies(const CliOptions& options);
  bool updateDependencies(const CliOptions& options);
  bool searchPackage(const std::string& packageName);
  bool removePackage(const std::string& packageName, PackageManager pm);

  // Enhanced dependency resolution
  DependencyResolution resolveDependencies(const std::vector<Dependency>& requested, const CliOptions& options);
  std::vector<Dependency> getRecommendedDependencies(TemplateType templateType);
  std::vector<Dependency> getTestingDependencies(TestFramework framework);
  std::vector<Dependency> getBuildDependencies(BuildSystem buildSystem);
  std::vector<Dependency> getFrameworkDependencies(const std::string& framework);

  // Package manager specific operations
  bool setupVcpkg(const std::string& projectPath);
  bool setupConan(const std::string& projectPath);
  bool setupSpack(const std::string& projectPath);
  bool setupHunter(const std::string& projectPath);
  bool setupCpm(const std::string& projectPath);
  bool setupFetchContent(const std::string& projectPath);
  bool setupMSYS2(const std::string& projectPath);

  // Enhanced configuration management
  bool generateVcpkgJson(const std::string& projectPath, const std::vector<Dependency>& deps);
  bool generateConanfile(const std::string& projectPath, const std::vector<Dependency>& deps);
  bool generateCMakePresets(const std::string& projectPath, PackageManager pm);
  bool generatePackageConfig(const std::string& projectPath, PackageManager pm, const std::vector<Dependency>& deps);
  bool generateLockfile(const std::string& projectPath, PackageManager pm, const std::vector<Dependency>& deps);

  // Advanced dependency validation
  bool validateDependencies(const std::vector<Dependency>& deps);
  bool checkDependencyCompatibility(const Dependency& dep1, const Dependency& dep2);
  bool validateVersionConstraints(const std::vector<Dependency>& deps);
  bool checkPlatformCompatibility(const Dependency& dep, const std::string& platform);
  bool checkCompilerCompatibility(const Dependency& dep, const std::string& compiler);

  // Interactive dependency selection
  std::vector<Dependency> selectDependenciesInteractively(const std::vector<Dependency>& available);
  Dependency configureDependencyInteractively(const std::string& packageName);

  // Enhanced package information
  std::optional<PackageInfo> getPackageInfo(const std::string& packageName, PackageManager pm);
  std::string getLatestVersion(const std::string& packageName, PackageManager pm);
  std::vector<std::string> getAvailableVersions(const std::string& packageName, PackageManager pm);
  bool isPackageAvailable(const std::string& packageName, PackageManager pm);
  std::vector<std::string> searchPackages(const std::string& query, PackageManager pm);

  // Version management
  bool compareVersions(const std::string& version1, const std::string& version2);
  std::string selectBestVersion(const std::vector<std::string>& availableVersions, const VersionConstraint& constraint);
  bool satisfiesConstraint(const std::string& version, const VersionConstraint& constraint);

  // Dependency analysis
  std::vector<std::string> analyzeDependencyTree(const std::vector<Dependency>& deps);
  std::vector<std::string> findCircularDependencies(const std::vector<Dependency>& deps);
  std::vector<std::string> findConflictingDependencies(const std::vector<Dependency>& deps);
  std::vector<Dependency> suggestAlternatives(const Dependency& dep);

  // Package manager detection and setup
  std::vector<PackageManager> detectAvailablePackageManagers();
  PackageManager recommendPackageManager(const CliOptions& options);
  bool isPackageManagerAvailable(PackageManager pm);
  bool installPackageManager(PackageManager pm);

  // Registry and cache management
  bool updatePackageRegistry(PackageManager pm);
  bool clearPackageCache(PackageManager pm);
  std::string getPackageCachePath(PackageManager pm);

  // Import/Export functionality
  bool exportDependencies(const std::string& filePath, const std::vector<Dependency>& deps, const std::string& format);
  std::vector<Dependency> importDependencies(const std::string& filePath, const std::string& format);

  // Reporting and analytics
  void generateDependencyReport(const std::string& projectPath, const std::vector<Dependency>& deps);
  std::unordered_map<std::string, std::string> getDependencyStatistics(const std::vector<Dependency>& deps);
  std::vector<std::string> checkForSecurityVulnerabilities(const std::vector<Dependency>& deps);
  std::vector<std::string> checkForLicenseCompatibility(const std::vector<Dependency>& deps);

private:
  DependencyManager() = default;

  // Internal helper methods
  PackageManagerConfig getPackageManagerConfig(PackageManager pm);
  std::string executeCommand(const std::string& command);
  bool writeConfigFile(const std::string& path, const std::string& content);
  std::string readConfigFile(const std::string& path);
  bool createDirectory(const std::string& path);

  // Enhanced dependency databases
  void initializeDependencyDatabase();
  void loadPackageRegistries();
  void initializePackageManagerConfigs();

  std::unordered_map<std::string, Dependency> commonDependencies_;
  std::unordered_map<TemplateType, std::vector<std::string>> templateDependencies_;
  std::unordered_map<TestFramework, std::vector<std::string>> testDependencies_;
  std::unordered_map<std::string, std::vector<std::string>> frameworkDependencies_;

  // Package registries and caches
  std::unordered_map<PackageManager, std::unordered_map<std::string, PackageInfo>> packageRegistries_;
  std::unordered_map<PackageManager, std::string> registryUrls_;
  std::unordered_map<PackageManager, std::string> cachePaths_;

  // Package manager configurations
  std::unordered_map<PackageManager, PackageManagerConfig> packageManagerConfigs_;

  // Dependency resolution helpers
  bool resolveDependencyTree(const std::vector<Dependency>& deps, std::vector<Dependency>& resolved, std::vector<std::string>& conflicts);
  std::vector<Dependency> flattenDependencyTree(const std::vector<Dependency>& deps);
  bool checkVersionCompatibility(const std::string& version1, const std::string& version2, const VersionConstraint& constraint);

  // Version parsing and comparison
  struct Version {
    int major, minor, patch;
    std::string prerelease;
    std::string build;

    static Version parse(const std::string& versionStr);
    bool operator<(const Version& other) const;
    bool operator==(const Version& other) const;
    std::string toString() const;
  };

  // Package manager specific helpers
  bool executeVcpkgCommand(const std::string& command, std::string& output);
  bool executeConanCommand(const std::string& command, std::string& output);
  bool executeSpackCommand(const std::string& command, std::string& output);

  // Configuration file generators
  std::string generateVcpkgJsonContent(const std::vector<Dependency>& deps);
  std::string generateConanfileContent(const std::vector<Dependency>& deps);
  std::string generateCMakePresetsContent(PackageManager pm);
  std::string generateRequirementsContent(const std::vector<Dependency>& deps, const std::string& format);

  // Validation helpers
  bool validateDependencyName(const std::string& name);
  bool validateVersionString(const std::string& version);
  bool isValidLicense(const std::string& license);

  // Security and license checking
  std::vector<std::string> knownVulnerabilities_;
  std::unordered_map<std::string, std::vector<std::string>> licenseCompatibility_;

  void loadSecurityDatabase();
  void loadLicenseCompatibilityRules();
};

// Utility functions for dependency management
namespace dependency_utils {
  // Version utilities
  bool isValidVersion(const std::string& version);
  std::string normalizeVersion(const std::string& version);
  int compareVersions(const std::string& v1, const std::string& v2);

  // Package name utilities
  std::string normalizePackageName(const std::string& name);
  bool isValidPackageName(const std::string& name);

  // Configuration file utilities
  std::string formatDependencyList(const std::vector<Dependency>& deps, const std::string& format);
  std::vector<Dependency> parseDependencyList(const std::string& content, const std::string& format);

  // Interactive utilities
  void printDependencyInfo(const Dependency& dep);
  void printDependencyList(const std::vector<Dependency>& deps);
  void printResolutionResult(const DependencyResolution& result);
  void printPackageInfo(const PackageInfo& info);
}

} // namespace utils
