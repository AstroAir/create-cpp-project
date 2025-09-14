#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace utils {

// Validation severity levels
enum class ValidationSeverity {
    Info,
    Warning,
    Error,
    Critical
};

// Validation issue structure
struct ValidationIssue {
    ValidationSeverity severity;
    std::string category;
    std::string message;
    std::string suggestion;
    std::filesystem::path filePath;
    size_t lineNumber = 0;
    std::string context;
};

// Validation result
struct ValidationResult {
    bool isValid = true;
    std::vector<ValidationIssue> issues;
    std::unordered_map<std::string, size_t> categoryCount;
    size_t totalIssues = 0;
    size_t criticalCount = 0;
    size_t errorCount = 0;
    size_t warningCount = 0;
    size_t infoCount = 0;
    
    bool hasCriticalIssues() const { return criticalCount > 0; }
    bool hasErrors() const { return errorCount > 0; }
    bool hasWarnings() const { return warningCount > 0; }
    operator bool() const { return isValid && criticalCount == 0 && errorCount == 0; }
};

// Project structure definition
struct ProjectStructure {
    std::vector<std::filesystem::path> requiredDirectories;
    std::vector<std::filesystem::path> requiredFiles;
    std::vector<std::filesystem::path> optionalDirectories;
    std::vector<std::filesystem::path> optionalFiles;
    std::unordered_map<std::string, std::vector<std::string>> filePatterns; // extension -> patterns
};

// Code quality metrics
struct CodeQualityMetrics {
    size_t totalLines = 0;
    size_t codeLines = 0;
    size_t commentLines = 0;
    size_t blankLines = 0;
    double commentRatio = 0.0;
    size_t totalFiles = 0;
    size_t headerFiles = 0;
    size_t sourceFiles = 0;
    size_t averageFunctionLength = 0;
    size_t maxFunctionLength = 0;
    size_t totalFunctions = 0;
    size_t totalClasses = 0;
    std::unordered_map<std::string, size_t> complexity; // file -> complexity score
};

// Dependency analysis
struct DependencyInfo {
    std::string name;
    std::string version;
    std::string source; // vcpkg, conan, system, etc.
    bool available = false;
    bool required = true;
    std::vector<std::string> dependsOn;
    std::string location;
};

struct DependencyAnalysis {
    std::vector<DependencyInfo> dependencies;
    std::vector<std::string> missingDependencies;
    std::vector<std::string> circularDependencies;
    std::vector<std::string> unusedDependencies;
    bool hasConflicts = false;
    std::vector<std::string> conflicts;
};

class ProjectValidator {
public:
    // Project structure validation
    static ValidationResult validateProjectStructure(const std::filesystem::path& projectPath);
    static ValidationResult validateDirectoryStructure(const std::filesystem::path& projectPath,
                                                      const ProjectStructure& expectedStructure);
    static ValidationResult validateFileNaming(const std::filesystem::path& projectPath);
    static ValidationResult validatePathLength(const std::filesystem::path& projectPath);
    
    // Build system validation
    static ValidationResult validateBuildSystem(const std::filesystem::path& projectPath);
    static ValidationResult validateCMakeFiles(const std::filesystem::path& projectPath);
    static ValidationResult validateXMakeFiles(const std::filesystem::path& projectPath);
    static ValidationResult validateMesonFiles(const std::filesystem::path& projectPath);
    static ValidationResult validateMakefiles(const std::filesystem::path& projectPath);
    
    // Source code validation
    static ValidationResult validateSourceCode(const std::filesystem::path& projectPath);
    static ValidationResult validateHeaderFiles(const std::filesystem::path& projectPath);
    static ValidationResult validateIncludeGuards(const std::filesystem::path& projectPath);
    static ValidationResult validateNamespaces(const std::filesystem::path& projectPath);
    static ValidationResult validateCodingStyle(const std::filesystem::path& projectPath);
    
    // Documentation validation
    static ValidationResult validateDocumentation(const std::filesystem::path& projectPath);
    static ValidationResult validateReadme(const std::filesystem::path& projectPath);
    static ValidationResult validateChangelog(const std::filesystem::path& projectPath);
    static ValidationResult validateCodeComments(const std::filesystem::path& projectPath);
    
    // Test validation
    static ValidationResult validateTestStructure(const std::filesystem::path& projectPath);
    static ValidationResult validateTestFiles(const std::filesystem::path& projectPath);
    static ValidationResult validateTestCoverage(const std::filesystem::path& projectPath);
    
    // Dependency validation
    static ValidationResult validateDependencies(const std::filesystem::path& projectPath);
    static DependencyAnalysis analyzeDependencies(const std::filesystem::path& projectPath);
    static ValidationResult validatePackageManagerFiles(const std::filesystem::path& projectPath);
    static ValidationResult checkForCircularDependencies(const std::filesystem::path& projectPath);
    
    // Configuration validation
    static ValidationResult validateConfiguration(const std::filesystem::path& projectPath);
    static ValidationResult validateEditorConfig(const std::filesystem::path& projectPath);
    static ValidationResult validateGitIgnore(const std::filesystem::path& projectPath);
    static ValidationResult validateCIConfig(const std::filesystem::path& projectPath);
    
    // License and legal validation
    static ValidationResult validateLicense(const std::filesystem::path& projectPath);
    static ValidationResult validateCopyright(const std::filesystem::path& projectPath);
    static ValidationResult validateThirdPartyLicenses(const std::filesystem::path& projectPath);
    
    // Security validation
    static ValidationResult validateSecurity(const std::filesystem::path& projectPath);
    static ValidationResult checkForSecrets(const std::filesystem::path& projectPath);
    static ValidationResult validateFilePermissions(const std::filesystem::path& projectPath);
    static ValidationResult checkForVulnerableCode(const std::filesystem::path& projectPath);
    
    // Code quality analysis
    static CodeQualityMetrics calculateCodeQualityMetrics(const std::filesystem::path& projectPath);
    static ValidationResult analyzeCodeComplexity(const std::filesystem::path& projectPath);
    static ValidationResult checkCodeDuplication(const std::filesystem::path& projectPath);
    static ValidationResult validateNamingConventions(const std::filesystem::path& projectPath);
    
    // Comprehensive validation
    static ValidationResult validateProject(const std::filesystem::path& projectPath,
                                          const std::vector<std::string>& validators = {});
    static ValidationResult validateProjectWithConfig(const std::filesystem::path& projectPath,
                                                     const std::filesystem::path& configFile);
    
    // Validation utilities
    static void addValidationRule(const std::string& name, 
                                 std::function<ValidationResult(const std::filesystem::path&)> validator);
    static void removeValidationRule(const std::string& name);
    static std::vector<std::string> getAvailableValidators();
    
    // Reporting and output
    static std::string formatValidationReport(const ValidationResult& result);
    static void saveValidationReport(const ValidationResult& result, 
                                   const std::filesystem::path& outputPath);
    static std::string generateSummaryReport(const ValidationResult& result);
    static std::string generateDetailedReport(const ValidationResult& result);
    
    // Issue management
    static ValidationResult filterIssuesBySeverity(const ValidationResult& result, 
                                                   ValidationSeverity minSeverity);
    static ValidationResult filterIssuesByCategory(const ValidationResult& result, 
                                                   const std::vector<std::string>& categories);
    static ValidationResult sortIssues(const ValidationResult& result);
    
    // Configuration and customization
    static void setValidationConfig(const std::unordered_map<std::string, std::string>& config);
    static void enableValidator(const std::string& validatorName);
    static void disableValidator(const std::string& validatorName);
    static void setValidationLevel(const std::string& level); // strict, normal, relaxed
    
private:
    static ValidationIssue createIssue(ValidationSeverity severity,
                                      const std::string& category,
                                      const std::string& message,
                                      const std::string& suggestion = "",
                                      const std::filesystem::path& filePath = "",
                                      size_t lineNumber = 0);
    
    static void addIssue(ValidationResult& result, const ValidationIssue& issue);
    static void updateCounts(ValidationResult& result);
    
    static std::vector<std::filesystem::path> findFilesByPattern(const std::filesystem::path& directory,
                                                                const std::string& pattern);
    static std::string readFileContent(const std::filesystem::path& filePath);
    static std::vector<std::string> readFileLines(const std::filesystem::path& filePath);
    
    static bool matchesPattern(const std::string& text, const std::string& pattern);
    static std::vector<std::string> extractIncludes(const std::string& content);
    static std::vector<std::string> extractNamespaces(const std::string& content);
    static size_t calculateCyclomaticComplexity(const std::string& content);
    
    // Built-in validation rules registry
    static std::unordered_map<std::string, std::function<ValidationResult(const std::filesystem::path&)>> validators_;
    static std::unordered_set<std::string> enabledValidators_;
    static std::unordered_map<std::string, std::string> validationConfig_;
    static std::string validationLevel_;
};

} // namespace utils
