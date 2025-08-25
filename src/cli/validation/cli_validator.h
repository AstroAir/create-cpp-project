#pragma once

#include "../types/cli_options.h"
#include <string>
#include <vector>
#include <functional>

// Validation result structure
struct ValidationResult {
  bool isValid = true;
  std::vector<std::string> errors;
  std::vector<std::string> warnings;

  void addError(const std::string& error) {
    errors.push_back(error);
    isValid = false;
  }

  void addWarning(const std::string& warning) {
    warnings.push_back(warning);
  }

  bool hasErrors() const { return !errors.empty(); }
  bool hasWarnings() const { return !warnings.empty(); }

  void merge(const ValidationResult& other) {
    if (!other.isValid) isValid = false;
    errors.insert(errors.end(), other.errors.begin(), other.errors.end());
    warnings.insert(warnings.end(), other.warnings.begin(), other.warnings.end());
  }
};

// CLI options validator
class CliValidator {
public:
  // Main validation method
  static ValidationResult validate(const CliOptions& options);

  // Category-specific validation
  static ValidationResult validateProjectDetails(const CliOptions& options);
  static ValidationResult validateBuildConfiguration(const CliOptions& options);
  static ValidationResult validateTestingConfiguration(const CliOptions& options);
  static ValidationResult validateDocumentationConfiguration(const CliOptions& options);
  static ValidationResult validateGitConfiguration(const CliOptions& options);
  static ValidationResult validateDevelopmentTools(const CliOptions& options);

  // Individual field validation
  static bool validateProjectName(const std::string& name, std::string& error);
  static bool validateProjectPath(const std::string& path, std::string& error);
  static bool validateGitUrl(const std::string& url, std::string& error);
  static bool validateEmail(const std::string& email, std::string& error);

  // Compatibility validation
  static ValidationResult validateCompatibility(const CliOptions& options);

  // Template-specific validation
  static ValidationResult validateTemplateRequirements(const CliOptions& options);

  // Configuration consistency validation
  static ValidationResult validateConsistency(const CliOptions& options);

private:
  // Helper validation functions
  static bool isValidIdentifier(const std::string& name);
  static bool isValidPath(const std::string& path);
  static bool isValidUrl(const std::string& url);
  static bool isValidEmailFormat(const std::string& email);

  // Template-specific validators
  static ValidationResult validateConsoleTemplate(const CliOptions& options);
  static ValidationResult validateLibraryTemplate(const CliOptions& options);
  static ValidationResult validateGuiTemplate(const CliOptions& options);
  static ValidationResult validateNetworkTemplate(const CliOptions& options);
  static ValidationResult validateGameTemplate(const CliOptions& options);

  // Compatibility checkers
  static bool isBuildSystemCompatible(BuildSystem build, PackageManager pkg);
  static bool isTestFrameworkCompatible(TestFramework test, BuildSystem build);
  static bool isCiSystemCompatible(CiSystem ci, BuildSystem build);
};

// Validation utilities
namespace validation_utils {

// Display validation results
void printValidationResult(const ValidationResult& result);
void printValidationSummary(const ValidationResult& result);

// Validation result formatting
std::string formatValidationErrors(const ValidationResult& result);
std::string formatValidationWarnings(const ValidationResult& result);

// Interactive validation
bool askUserToFixErrors(const ValidationResult& result);
CliOptions promptUserToFixConfiguration(const CliOptions& options, const ValidationResult& result);

} // namespace validation_utils
