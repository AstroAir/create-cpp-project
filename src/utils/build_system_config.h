#pragma once

#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <unordered_map>

namespace utils {

// C++ Standard versions
enum class CppStandard {
  Cpp11,
  Cpp14,
  Cpp17,
  Cpp20,
  Cpp23,
  Latest
};

// Build optimization levels
enum class OptimizationLevel {
  None,      // -O0
  Debug,     // -Og
  Size,      // -Os
  Speed,     // -O2
  Aggressive // -O3
};

// Build configuration types
enum class BuildConfiguration {
  Debug,
  Release,
  RelWithDebInfo,
  MinSizeRel,
  Custom
};

// Compiler-specific settings
struct CompilerSettings {
  std::vector<std::string> cxxFlags;
  std::vector<std::string> linkFlags;
  std::vector<std::string> definitions;
  std::vector<std::string> warningFlags;
  std::map<std::string, std::string> options;
  bool enableLTO = false;
  bool enablePCH = false; // Precompiled headers
  bool enableCcache = false;
  bool enableClangTidy = false;
  bool enableCppcheck = false;
};

// Platform-specific build settings
struct PlatformSettings {
  std::vector<std::string> libraries;
  std::vector<std::string> frameworks; // macOS specific
  std::vector<std::string> systemLibraries;
  std::map<std::string, std::string> environmentVars;
  std::vector<std::string> linkDirectories;
  std::vector<std::string> includeDirectories;
};

// Build system configuration
struct BuildSystemConfig {
  BuildSystem buildSystem;
  CppStandard cppStandard;
  OptimizationLevel optimizationLevel;
  BuildConfiguration buildConfiguration;
  CompilerSettings compilerSettings;
  PlatformSettings platformSettings;
  
  // Advanced options
  bool enableTesting = true;
  bool enableBenchmarking = false;
  bool enableProfiling = false;
  bool enableSanitizers = false;
  bool enableCoverage = false;
  bool enableDocumentation = true;
  bool enableExamples = true;
  bool enableInstall = true;
  bool enablePackaging = false;
  
  // Cross-compilation settings
  std::string targetTriple;
  std::string sysroot;
  std::string toolchainFile;
  
  // Build parallelization
  int parallelJobs = 0; // 0 = auto-detect
  bool enableDistributedBuild = false;
  
  // Output settings
  std::string outputDirectory = "build";
  std::string installPrefix = "/usr/local";
  bool staticLinking = false;
  bool sharedLibraries = true;
};

// Build system configuration manager
class BuildSystemConfigManager {
public:
  static BuildSystemConfigManager& getInstance();
  
  // Configuration creation and validation
  BuildSystemConfig createDefaultConfig(BuildSystem buildSystem, TemplateType templateType);
  BuildSystemConfig createOptimizedConfig(BuildSystem buildSystem, TemplateType templateType, OptimizationLevel level);
  bool validateConfig(const BuildSystemConfig& config);
  
  // Interactive configuration
  BuildSystemConfig configureInteractively(const CliOptions& options);
  CppStandard selectCppStandard(CppStandard defaultStandard = CppStandard::Cpp17);
  OptimizationLevel selectOptimizationLevel(OptimizationLevel defaultLevel = OptimizationLevel::Speed);
  CompilerSettings configureCompilerSettings(const std::string& compiler);
  
  // Build system specific generators
  std::string generateCMakeConfig(const BuildSystemConfig& config, const std::string& projectName);
  std::string generateMesonConfig(const BuildSystemConfig& config, const std::string& projectName);
  std::string generateBazelConfig(const BuildSystemConfig& config, const std::string& projectName);
  std::string generateXMakeConfig(const BuildSystemConfig& config, const std::string& projectName);
  std::string generatePremakeConfig(const BuildSystemConfig& config, const std::string& projectName);
  std::string generateMakefileConfig(const BuildSystemConfig& config, const std::string& projectName);
  std::string generateNinjaConfig(const BuildSystemConfig& config, const std::string& projectName);
  
  // Toolchain and preset generation
  std::string generateCMakePresets(const BuildSystemConfig& config, const std::string& projectName);
  std::string generateCMakeToolchain(const BuildSystemConfig& config);
  std::string generateConanProfile(const BuildSystemConfig& config);
  std::string generateVcpkgTriplet(const BuildSystemConfig& config);
  
  // Compiler detection and configuration
  std::vector<std::string> detectAvailableCompilers();
  std::string detectDefaultCompiler();
  CompilerSettings getDefaultCompilerSettings(const std::string& compiler, CppStandard standard);
  
  // Platform-specific utilities
  PlatformSettings getDefaultPlatformSettings();
  std::vector<std::string> getPlatformSpecificLibraries();
  std::string getPlatformSpecificFlags();
  
  // Build optimization
  std::vector<std::string> getOptimizationFlags(OptimizationLevel level, const std::string& compiler);
  std::vector<std::string> getWarningFlags(const std::string& compiler, bool strict = true);
  std::vector<std::string> getSanitizerFlags(const std::string& compiler);
  std::vector<std::string> getCoverageFlags(const std::string& compiler);
  
  // Configuration templates
  BuildSystemConfig getTemplateConfig(TemplateType templateType, BuildSystem buildSystem);
  std::map<std::string, BuildSystemConfig> getPresetConfigs();
  
  // Import/Export functionality
  bool saveConfig(const BuildSystemConfig& config, const std::string& filePath);
  std::optional<BuildSystemConfig> loadConfig(const std::string& filePath);
  std::string exportConfigToJson(const BuildSystemConfig& config);
  std::optional<BuildSystemConfig> importConfigFromJson(const std::string& json);
  
  // Validation and compatibility
  bool isCompilerCompatible(const std::string& compiler, CppStandard standard);
  bool isPlatformSupported(const std::string& platform, BuildSystem buildSystem);
  std::vector<std::string> validateConfiguration(const BuildSystemConfig& config);
  
  // Build system capabilities
  std::vector<std::string> getSupportedFeatures(BuildSystem buildSystem);
  bool supportsFeature(BuildSystem buildSystem, const std::string& feature);
  std::string getBuildSystemVersion(BuildSystem buildSystem);
  
private:
  BuildSystemConfigManager() = default;
  
  // Internal configuration helpers
  void initializeDefaultConfigs();
  void loadCompilerDatabase();
  void loadPlatformDatabase();
  
  // Configuration databases
  std::map<BuildSystem, BuildSystemConfig> defaultConfigs_;
  std::map<std::string, CompilerSettings> compilerDatabase_;
  std::map<std::string, PlatformSettings> platformDatabase_;
  std::map<TemplateType, std::map<BuildSystem, BuildSystemConfig>> templateConfigs_;
  
  // Utility methods
  std::string cppStandardToString(CppStandard standard);
  std::string optimizationLevelToString(OptimizationLevel level);
  std::string buildConfigurationToString(BuildConfiguration config);
  
  CppStandard stringToCppStandard(const std::string& str);
  OptimizationLevel stringToOptimizationLevel(const std::string& str);
  BuildConfiguration stringToBuildConfiguration(const std::string& str);
  
  // Platform detection
  std::string detectCurrentPlatform();
  std::string detectCurrentArchitecture();
  std::string detectCurrentCompiler();
  
  // Build system specific helpers
  std::string generateCMakeCompilerFlags(const CompilerSettings& settings);
  std::string generateCMakeLinkFlags(const CompilerSettings& settings);
  std::string generateMesonCompilerArgs(const CompilerSettings& settings);
  std::string generateBazelCopts(const CompilerSettings& settings);
};

// Utility functions for build system configuration
namespace build_config_utils {
  // String conversion utilities
  std::string toString(CppStandard standard);
  std::string toString(OptimizationLevel level);
  std::string toString(BuildConfiguration config);
  
  CppStandard parseCppStandard(const std::string& str);
  OptimizationLevel parseOptimizationLevel(const std::string& str);
  BuildConfiguration parseBuildConfiguration(const std::string& str);
  
  // Validation utilities
  bool isValidCppStandard(const std::string& str);
  bool isValidOptimizationLevel(const std::string& str);
  bool isValidBuildConfiguration(const std::string& str);
  
  // Display utilities
  void printBuildSystemConfig(const BuildSystemConfig& config);
  void printCompilerSettings(const CompilerSettings& settings);
  void printPlatformSettings(const PlatformSettings& settings);
  
  // Comparison utilities
  bool isConfigurationCompatible(const BuildSystemConfig& config1, const BuildSystemConfig& config2);
  std::vector<std::string> findConfigurationDifferences(const BuildSystemConfig& config1, const BuildSystemConfig& config2);
}

} // namespace utils
