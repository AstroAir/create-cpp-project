#include "config_validator.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <regex>

#include "../utils/string_utils.h"
#include "../utils/terminal_utils.h"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <TargetConditionals.h>
#elif __linux__
#include <unistd.h>
#endif

namespace config {

// ValidationResult helper methods
bool ValidationResult::hasErrors() const {
    return std::any_of(messages.begin(), messages.end(), [](const ValidationMessage& msg) {
        return msg.severity == ValidationSeverity::Error ||
               msg.severity == ValidationSeverity::Critical;
    });
}

bool ValidationResult::hasWarnings() const {
    return std::any_of(messages.begin(), messages.end(), [](const ValidationMessage& msg) {
        return msg.severity == ValidationSeverity::Warning;
    });
}

std::vector<ValidationMessage> ValidationResult::getErrors() const {
    std::vector<ValidationMessage> errors;
    std::copy_if(messages.begin(), messages.end(), std::back_inserter(errors),
                 [](const ValidationMessage& msg) {
                     return msg.severity == ValidationSeverity::Error ||
                            msg.severity == ValidationSeverity::Critical;
                 });
    return errors;
}

std::vector<ValidationMessage> ValidationResult::getWarnings() const {
    std::vector<ValidationMessage> warnings;
    std::copy_if(messages.begin(), messages.end(), std::back_inserter(warnings),
                 [](const ValidationMessage& msg) {
                     return msg.severity == ValidationSeverity::Warning;
                 });
    return warnings;
}

void ValidationResult::addMessage(ValidationSeverity severity, const std::string& category,
                                  const std::string& message, const std::string& suggestion,
                                  const std::string& component) {
    messages.push_back({severity, category, message, suggestion, component});
    if (severity == ValidationSeverity::Error || severity == ValidationSeverity::Critical) {
        isValid = false;
    }
}

// ConfigValidator implementation
ConfigValidator& ConfigValidator::getInstance() {
    static ConfigValidator instance;
    return instance;
}

ValidationResult ConfigValidator::validateConfiguration(const CliOptions& options) const {
    ValidationResult result;

    // Validate project name
    auto nameResult = validateProjectName(options.projectName);
    result.messages.insert(result.messages.end(), nameResult.messages.begin(),
                           nameResult.messages.end());
    if (!nameResult.isValid)
        result.isValid = false;

    // Validate template compatibility
    auto templateResult = validateTemplateCompatibility(options);
    result.messages.insert(result.messages.end(), templateResult.messages.begin(),
                           templateResult.messages.end());
    if (!templateResult.isValid)
        result.isValid = false;

    // Validate build system compatibility
    auto buildResult = validateBuildSystemCompatibility(options);
    result.messages.insert(result.messages.end(), buildResult.messages.begin(),
                           buildResult.messages.end());
    if (!buildResult.isValid)
        result.isValid = false;

    // Validate package manager compatibility
    auto packageResult = validatePackageManagerCompatibility(options);
    result.messages.insert(result.messages.end(), packageResult.messages.begin(),
                           packageResult.messages.end());
    if (!packageResult.isValid)
        result.isValid = false;

    // Platform-specific validation
    if (platformSpecificValidation_) {
        auto platformResult = validatePlatformCompatibility(options);
        result.messages.insert(result.messages.end(), platformResult.messages.begin(),
                               platformResult.messages.end());
        if (!platformResult.isValid)
            result.isValid = false;
    }

    // Run custom validation rules
    for (const auto& [name, rule] : customRules_) {
        auto ruleResult = rule(options);
        result.messages.insert(result.messages.end(), ruleResult.messages.begin(),
                               ruleResult.messages.end());
        if (!ruleResult.isValid)
            result.isValid = false;
    }

    return result;
}

ValidationResult ConfigValidator::validateProjectName(const std::string& name) const {
    ValidationResult result;

    if (name.empty()) {
        result.addMessage(ValidationSeverity::Error, "Project Name", "Project name cannot be empty",
                          "Provide a valid project name");
        return result;
    }

    if (!isValidProjectName(name)) {
        result.addMessage(ValidationSeverity::Error, "Project Name",
                          "Invalid project name: " + name,
                          "Use only alphanumeric characters, hyphens, and underscores");
        return result;
    }

    if (isReservedKeyword(name)) {
        result.addMessage(ValidationSeverity::Error, "Project Name",
                          "Project name '" + name + "' is a reserved keyword",
                          "Choose a different name");
        return result;
    }

    if (!isValidLength(name)) {
        result.addMessage(ValidationSeverity::Warning, "Project Name",
                          "Project name is very long or very short",
                          "Consider using a name between 3-50 characters");
    }

    return result;
}

ValidationResult ConfigValidator::validateTemplateCompatibility(const CliOptions& options) const {
    ValidationResult result;

    // Check template with build system
    auto buildCompat = checkTemplateWithBuildSystem(options.templateType, options.buildSystem);
    if (!buildCompat.isCompatible) {
        result.addMessage(
                ValidationSeverity::Error, "Template Compatibility",
                "Template type incompatible with build system: " + buildCompat.reason,
                "Consider using: " + ::utils::StringUtils::join(buildCompat.alternatives, ", "));
    }

    // Check template with package manager
    auto packageCompat =
            checkTemplateWithPackageManager(options.templateType, options.packageManager);
    if (!packageCompat.isCompatible) {
        result.addMessage(ValidationSeverity::Warning, "Template Compatibility",
                          "Template may have issues with package manager: " + packageCompat.reason,
                          packageCompat.workaround);
    }

    return result;
}

ValidationResult ConfigValidator::validateBuildSystemCompatibility(
        const CliOptions& options) const {
    ValidationResult result;

    // Check if build system tools are available
    if (!checkBuildToolAvailability(options.buildSystem)) {
        result.addMessage(ValidationSeverity::Error, "Build System",
                          "Build system tools not found on system",
                          "Install required build tools for " +
                                  std::string(cli_enums::to_string(options.buildSystem)));
    }

    // Check compiler availability
    if (!checkCompilerAvailability(options.buildSystem)) {
        result.addMessage(ValidationSeverity::Warning, "Build System",
                          "Recommended compiler not found", "Install a compatible C++ compiler");
    }

    return result;
}

ValidationResult ConfigValidator::validatePackageManagerCompatibility(
        const CliOptions& options) const {
    ValidationResult result;

    if (options.packageManager != PackageManager::None) {
        if (!checkPackageManagerAvailability(options.packageManager)) {
            result.addMessage(ValidationSeverity::Warning, "Package Manager",
                              "Package manager not found on system",
                              "Install " +
                                      std::string(cli_enums::to_string(options.packageManager)) +
                                      " or use --package none");
        }
    }

    return result;
}

ValidationResult ConfigValidator::validatePlatformCompatibility(const CliOptions& options) const {
    ValidationResult result;

    std::string platform = getCurrentPlatform();

    // Platform-specific template checks
    if (options.templateType == TemplateType::Embedded) {
        if (platform == "Windows") {
            result.addMessage(ValidationSeverity::Info, "Platform",
                              "Embedded development on Windows may require additional setup",
                              "Consider using WSL or a Linux VM for embedded development");
        }
    }

    // Build system platform checks
    if (options.buildSystem == BuildSystem::Make && platform == "Windows") {
        result.addMessage(ValidationSeverity::Warning, "Platform",
                          "Make is not natively available on Windows",
                          "Consider using CMake or install Make through MSYS2/MinGW");
    }

    return result;
}

bool ConfigValidator::isValidProjectName(const std::string& name) const {
    // Check for valid characters (alphanumeric, hyphens, underscores)
    std::regex validPattern("^[a-zA-Z0-9_-]+$");
    return std::regex_match(name, validPattern) && hasValidCharacters(name);
}

bool ConfigValidator::isReservedKeyword(const std::string& name) const {
    static const std::vector<std::string> reserved = {
            "main",     "test",   "src",     "include",   "lib",     "bin",    "build", "cmake",
            "makefile", "readme", "license", "copyright", "version", "config", "con",   "prn",
            "aux",      "nul",    "com1",    "com2",      "com3",    "com4",   "com5",  "com6",
            "com7",     "com8",   "com9",    "lpt1",      "lpt2",    "lpt3",   "lpt4",  "lpt5",
            "lpt6",     "lpt7",   "lpt8",    "lpt9"  // Windows reserved names
    };

    std::string lowerName = ::utils::StringUtils::toLower(name);
    return std::find(reserved.begin(), reserved.end(), lowerName) != reserved.end();
}

bool ConfigValidator::hasValidCharacters(const std::string& name) const {
    // Must start with letter or underscore
    if (name.empty() || (!std::isalpha(name[0]) && name[0] != '_')) {
        return false;
    }

    // Cannot end with hyphen
    if (name.back() == '-') {
        return false;
    }

    return true;
}

bool ConfigValidator::isValidLength(const std::string& name) const {
    return name.length() >= 3 && name.length() <= 50;
}

std::string ConfigValidator::getCurrentPlatform() const {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#elif __linux__
    return "Linux";
#else
    return "Unknown";
#endif
}

bool ConfigValidator::checkBuildToolAvailability(BuildSystem buildSystem) const {
    (void)buildSystem;  // TODO: Implement build tool availability checks
    // This would typically check if the build tools are installed
    // For now, return true as a placeholder
    return true;
}

bool ConfigValidator::checkCompilerAvailability(BuildSystem buildSystem) const {
    (void)buildSystem;  // TODO: Implement compiler availability checks
    // This would typically check for GCC, Clang, MSVC availability
    // For now, return true as a placeholder
    return true;
}

bool ConfigValidator::checkPackageManagerAvailability(PackageManager packageManager) const {
    (void)packageManager;  // TODO: Implement package manager availability checks
    // This would typically check if vcpkg, conan, etc. are installed
    // For now, return true as a placeholder
    return true;
}

CompatibilityInfo ConfigValidator::checkTemplateWithBuildSystem(TemplateType template_type,
                                                                BuildSystem build_system) const {
    CompatibilityInfo info;
    info.isCompatible = true;  // Default to compatible

    // Check specific incompatibilities
    if (template_type == TemplateType::Embedded && build_system == BuildSystem::Meson) {
        info.isCompatible = false;
        info.reason = "Embedded projects typically require CMake for cross-compilation support";
        info.alternatives = {"cmake"};
        info.workaround = "Use CMake with toolchain files for embedded development";
    }

    if (template_type == TemplateType::WebService && build_system == BuildSystem::Make) {
        info.isCompatible = false;
        info.reason = "WebService projects require modern build systems for dependency management";
        info.alternatives = {"cmake", "meson"};
        info.workaround = "Use CMake or Meson for better dependency handling";
    }

    return info;
}

CompatibilityInfo ConfigValidator::checkTemplateWithPackageManager(
        TemplateType template_type, PackageManager package_manager) const {
    CompatibilityInfo info;
    info.isCompatible = true;  // Default to compatible

    // Check specific recommendations
    if (template_type == TemplateType::Embedded && package_manager != PackageManager::None) {
        info.isCompatible = true;  // Still compatible but not recommended
        info.reason = "Embedded projects often use custom dependency management";
        info.alternatives = {"none"};
        info.workaround = "Consider using git submodules or custom build scripts";
    }

    if (template_type == TemplateType::WebService && package_manager == PackageManager::None) {
        info.isCompatible = true;  // Compatible but not optimal
        info.reason = "WebService projects benefit from package managers for dependencies";
        info.alternatives = {"vcpkg", "conan"};
        info.workaround = "Manual dependency management will require more setup";
    }

    return info;
}

CompatibilityInfo ConfigValidator::checkBuildSystemWithPackageManager(
        BuildSystem build_system, PackageManager package_manager) const {
    CompatibilityInfo info;
    info.isCompatible = true;  // Default to compatible

    // Check specific combinations
    if (build_system == BuildSystem::CMake && package_manager == PackageManager::Vcpkg) {
        info.isCompatible = true;
        info.reason = "Excellent integration between CMake and vcpkg";
    }

    if (build_system == BuildSystem::Meson && package_manager == PackageManager::Vcpkg) {
        info.isCompatible = true;
        info.reason = "Good integration with some manual configuration";
        info.workaround = "May require manual pkg-config setup";
    }

    return info;
}

CompatibilityInfo ConfigValidator::checkTestFrameworkWithBuildSystem(
        TestFramework test_framework, BuildSystem build_system) const {
    CompatibilityInfo info;
    info.isCompatible = true;  // Default to compatible

    // All test frameworks work with CMake
    if (build_system == BuildSystem::CMake) {
        info.isCompatible = true;
        info.reason = "CMake has excellent support for all test frameworks";
    }

    // Check specific combinations
    if (test_framework == TestFramework::GTest && build_system == BuildSystem::Meson) {
        info.isCompatible = true;
        info.reason = "Meson has built-in support for Google Test";
    }

    return info;
}

void ConfigValidator::setStrictMode(bool enabled) {
    strictMode_ = enabled;
}

// Validation utilities
namespace validation_utils {
std::string severityToString(ValidationSeverity severity) {
    switch (severity) {
        case ValidationSeverity::Info:
            return "INFO";
        case ValidationSeverity::Warning:
            return "WARNING";
        case ValidationSeverity::Error:
            return "ERROR";
        case ValidationSeverity::Critical:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

std::string formatValidationMessage(const ValidationMessage& message) {
    std::string result = "[" + severityToString(message.severity) + "] ";
    if (!message.category.empty()) {
        result += message.category + ": ";
    }
    result += message.message;
    if (!message.suggestion.empty()) {
        result += " (Suggestion: " + message.suggestion + ")";
    }
    return result;
}

void printValidationResult(const ValidationResult& result) {
    if (result.isValid && result.messages.empty()) {
        ::utils::TerminalUtils::showSuccess("Configuration validation passed");
        return;
    }

    for (const auto& message : result.messages) {
        std::string formatted = formatValidationMessage(message);

        switch (message.severity) {
            case ValidationSeverity::Info:
                ::utils::TerminalUtils::showInfo(formatted);
                break;
            case ValidationSeverity::Warning:
                ::utils::TerminalUtils::showWarning(formatted);
                break;
            case ValidationSeverity::Error:
            case ValidationSeverity::Critical:
                ::utils::TerminalUtils::showError(formatted);
                break;
        }
    }

    if (!result.isValid) {
        ::utils::TerminalUtils::showError("Configuration validation failed");
    }
}
}  // namespace validation_utils

}  // namespace config
