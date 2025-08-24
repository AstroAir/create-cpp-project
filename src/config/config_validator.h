#pragma once

#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace config {

// Validation severity levels
enum class ValidationSeverity {
    Info,
    Warning,
    Error,
    Critical
};

// Validation message
struct ValidationMessage {
    ValidationSeverity severity;
    std::string category;
    std::string message;
    std::string suggestion;
    std::string component; // Which component this relates to
};

// Validation result
struct ValidationResult {
    bool isValid = true;
    std::vector<ValidationMessage> messages;
    
    // Helper methods
    bool hasErrors() const;
    bool hasWarnings() const;
    std::vector<ValidationMessage> getErrors() const;
    std::vector<ValidationMessage> getWarnings() const;
    void addMessage(ValidationSeverity severity, const std::string& category, 
                   const std::string& message, const std::string& suggestion = "",
                   const std::string& component = "");
};

// Configuration compatibility matrix
struct CompatibilityInfo {
    bool isCompatible = true;
    std::string reason;
    std::vector<std::string> alternatives;
    std::string workaround;
};

class ConfigValidator {
public:
    static ConfigValidator& getInstance();
    
    // Main validation methods
    ValidationResult validateConfiguration(const CliOptions& options) const;
    ValidationResult validateProjectName(const std::string& name) const;
    ValidationResult validateTemplateCompatibility(const CliOptions& options) const;
    ValidationResult validateBuildSystemCompatibility(const CliOptions& options) const;
    ValidationResult validatePackageManagerCompatibility(const CliOptions& options) const;
    ValidationResult validateTestFrameworkCompatibility(const CliOptions& options) const;
    ValidationResult validateEditorCompatibility(const CliOptions& options) const;
    ValidationResult validateCICompatibility(const CliOptions& options) const;
    
    // Compatibility checks
    CompatibilityInfo checkTemplateWithBuildSystem(TemplateType template_type, BuildSystem build_system) const;
    CompatibilityInfo checkBuildSystemWithPackageManager(BuildSystem build_system, PackageManager package_manager) const;
    CompatibilityInfo checkTemplateWithPackageManager(TemplateType template_type, PackageManager package_manager) const;
    CompatibilityInfo checkTestFrameworkWithBuildSystem(TestFramework test_framework, BuildSystem build_system) const;
    
    // Platform-specific validation
    ValidationResult validatePlatformCompatibility(const CliOptions& options) const;
    ValidationResult validateWindowsCompatibility(const CliOptions& options) const;
    ValidationResult validateLinuxCompatibility(const CliOptions& options) const;
    ValidationResult validateMacOSCompatibility(const CliOptions& options) const;
    
    // Dependency validation
    ValidationResult validateDependencies(const CliOptions& options) const;
    ValidationResult validateSystemRequirements(const CliOptions& options) const;
    
    // Configuration suggestions
    std::vector<std::string> suggestImprovements(const CliOptions& options) const;
    std::vector<std::string> suggestAlternatives(const CliOptions& options) const;
    CliOptions suggestOptimalConfiguration(const CliOptions& baseOptions) const;
    
    // Custom validation rules
    using ValidationRule = std::function<ValidationResult(const CliOptions&)>;
    void addCustomRule(const std::string& name, ValidationRule rule);
    void removeCustomRule(const std::string& name);
    std::vector<std::string> listCustomRules() const;
    
    // Configuration analysis
    std::map<std::string, std::string> analyzeConfiguration(const CliOptions& options) const;
    std::string generateConfigurationReport(const CliOptions& options) const;
    
    // Validation settings
    void setStrictMode(bool enabled);
    void setValidationLevel(ValidationSeverity minLevel);
    void enablePlatformSpecificValidation(bool enabled);
    
private:
    ConfigValidator() = default;
    
    std::map<std::string, ValidationRule> customRules_;
    bool strictMode_ = false;
    ValidationSeverity minValidationLevel_ = ValidationSeverity::Warning;
    bool platformSpecificValidation_ = true;
    
    // Internal validation helpers
    bool isValidProjectName(const std::string& name) const;
    bool isReservedKeyword(const std::string& name) const;
    bool hasValidCharacters(const std::string& name) const;
    bool isValidLength(const std::string& name) const;
    
    // Platform detection
    std::string getCurrentPlatform() const;
    bool isPlatformSupported(const std::string& platform) const;
    
    // Compatibility matrices
    void initializeCompatibilityMatrices();
    std::map<std::pair<TemplateType, BuildSystem>, CompatibilityInfo> templateBuildCompatibility_;
    std::map<std::pair<BuildSystem, PackageManager>, CompatibilityInfo> buildPackageCompatibility_;
    std::map<std::pair<TemplateType, PackageManager>, CompatibilityInfo> templatePackageCompatibility_;
    std::map<std::pair<TestFramework, BuildSystem>, CompatibilityInfo> testBuildCompatibility_;
    
    // Validation message helpers
    ValidationMessage createMessage(ValidationSeverity severity, const std::string& category,
                                  const std::string& message, const std::string& suggestion = "",
                                  const std::string& component = "") const;
    
    // System requirement checks
    bool checkCompilerAvailability(BuildSystem buildSystem) const;
    bool checkBuildToolAvailability(BuildSystem buildSystem) const;
    bool checkPackageManagerAvailability(PackageManager packageManager) const;
    
    // Configuration optimization
    CliOptions optimizeForPerformance(const CliOptions& options) const;
    CliOptions optimizeForDevelopment(const CliOptions& options) const;
    CliOptions optimizeForProduction(const CliOptions& options) const;
};

// Validation utilities
namespace validation_utils {
    std::string severityToString(ValidationSeverity severity);
    std::string formatValidationMessage(const ValidationMessage& message);
    void printValidationResult(const ValidationResult& result);
    bool isConfigurationRecommended(const CliOptions& options);
    std::vector<std::string> getCommonIssues(const CliOptions& options);
}

// Pre-defined validation rule sets
namespace validation_rules {
    extern const std::map<std::string, ConfigValidator::ValidationRule> BASIC_RULES;
    extern const std::map<std::string, ConfigValidator::ValidationRule> ADVANCED_RULES;
    extern const std::map<std::string, ConfigValidator::ValidationRule> ENTERPRISE_RULES;
}

} // namespace config
