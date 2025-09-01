#include "cli_validator.h"

#include <spdlog/fmt/fmt.h>

#include <filesystem>
#include <iostream>
#include <regex>

#include "../../utils/string_utils.h"
#include "../../utils/terminal_utils.h"
#include "../input/user_input.h"

using namespace utils;

// Main validation method
ValidationResult CliValidator::validate(const CliOptions& options) {
    ValidationResult result;

    // Validate different categories
    result.merge(validateProjectDetails(options));
    result.merge(validateBuildConfiguration(options));
    result.merge(validateTestingConfiguration(options));
    result.merge(validateDocumentationConfiguration(options));
    result.merge(validateGitConfiguration(options));
    result.merge(validateDevelopmentTools(options));

    // Validate compatibility and consistency
    result.merge(validateCompatibility(options));
    result.merge(validateTemplateRequirements(options));
    result.merge(validateConsistency(options));

    return result;
}

// Project details validation
ValidationResult CliValidator::validateProjectDetails(const CliOptions& options) {
    ValidationResult result;
    std::string error;

    // Validate project name
    if (!validateProjectName(options.projectName, error)) {
        result.addError("Project name: " + error);
    }

    // Validate custom template path if provided
    if (!options.customTemplatePath.empty()) {
        if (!std::filesystem::exists(options.customTemplatePath)) {
            result.addError("Custom template path does not exist: " +
                            options.customTemplatePath.string());
        }
    }

    return result;
}

// Build configuration validation
ValidationResult CliValidator::validateBuildConfiguration(const CliOptions& options) {
    ValidationResult result;

    // Check build system and package manager compatibility
    if (!isBuildSystemCompatible(options.buildSystem, options.packageManager)) {
        result.addWarning(fmt::format(
                "Build system '{}' may have limited compatibility with package manager '{}'",
                cli_enums::to_string(options.buildSystem),
                cli_enums::to_string(options.packageManager)));
    }

    // Validate network library for network templates
    if (options.templateType == TemplateType::Network) {
        if (!options.networkLibrary.has_value() || options.networkLibrary->empty()) {
            result.addError("Network template requires a network library to be specified");
        } else {
            std::vector<std::string> validNetworkLibs = {"asio", "boost", "poco"};
            if (std::find(validNetworkLibs.begin(), validNetworkLibs.end(),
                          *options.networkLibrary) == validNetworkLibs.end()) {
                result.addWarning("Unknown network library: " + *options.networkLibrary);
            }
        }
    }

    return result;
}

// Testing configuration validation
ValidationResult CliValidator::validateTestingConfiguration(const CliOptions& options) {
    ValidationResult result;

    if (options.includeTests) {
        // Check test framework compatibility with build system
        if (!isTestFrameworkCompatible(options.testFramework, options.buildSystem)) {
            result.addWarning(
                    fmt::format("Test framework '{}' may require additional configuration with "
                                "build system '{}'",
                                cli_enums::to_string(options.testFramework),
                                cli_enums::to_string(options.buildSystem)));
        }

        // Warn if no test framework selected
        if (options.testFramework == TestFramework::None) {
            result.addWarning("Tests are enabled but no test framework is selected");
        }
    }

    return result;
}

// Documentation configuration validation
ValidationResult CliValidator::validateDocumentationConfiguration(const CliOptions& options) {
    ValidationResult result;

    if (options.includeDocumentation) {
        // Validate documentation formats
        for (const auto& format : options.docFormats) {
            std::vector<std::string> validFormats = {"markdown", "html", "pdf", "latex"};
            if (std::find(validFormats.begin(), validFormats.end(), format) == validFormats.end()) {
                result.addWarning("Unknown documentation format: " + format);
            }
        }

        // Validate documentation types
        for (const auto& type : options.docTypes) {
            std::vector<std::string> validTypes = {"readme", "api", "user", "developer",
                                                   "tutorial"};
            if (std::find(validTypes.begin(), validTypes.end(), type) == validTypes.end()) {
                result.addWarning("Unknown documentation type: " + type);
            }
        }

        // Check Doxygen configuration
        if (options.generateDoxygen && options.doxygenTheme.empty()) {
            result.addWarning("Doxygen is enabled but no theme is specified, using default");
        }
    }

    return result;
}

// Git configuration validation
ValidationResult CliValidator::validateGitConfiguration(const CliOptions& options) {
    ValidationResult result;
    std::string error;

    if (options.initGit) {
        // Validate Git remote URL if provided
        if (!options.gitRemoteUrl.empty() && !validateGitUrl(options.gitRemoteUrl, error)) {
            result.addError("Git remote URL: " + error);
        }

        // Validate Git user email if provided
        if (!options.gitUserEmail.empty() && !validateEmail(options.gitUserEmail, error)) {
            result.addError("Git user email: " + error);
        }

        // Validate Git user name
        if (!options.gitUserName.empty() && options.gitUserName.length() > 100) {
            result.addWarning("Git user name is very long (>100 characters)");
        }

        // Check workflow and branch strategy compatibility
        if (options.gitWorkflow == GitWorkflow::GitFlow &&
            options.gitBranchStrategy != GitBranchStrategy::GitFlow) {
            result.addWarning("GitFlow workflow is selected but branch strategy is not GitFlow");
        }
    }

    return result;
}

// Development tools validation
ValidationResult CliValidator::validateDevelopmentTools(const CliOptions& options) {
    ValidationResult result;

    // Validate GUI frameworks for GUI templates
    if (options.templateType == TemplateType::Gui || options.templateType == TemplateType::QtApp) {
        if (options.guiFrameworks.empty()) {
            result.addWarning("GUI template selected but no GUI frameworks specified");
        }
    }

    // Validate game frameworks for game templates
    if (options.templateType == TemplateType::GameEngine ||
        options.templateType == TemplateType::SfmlApp) {
        if (options.gameFrameworks.empty()) {
            result.addWarning("Game template selected but no game frameworks specified");
        }
    }

    // Check CI system compatibility
    for (const auto& ci : options.ciOptions) {
        if (!isCiSystemCompatible(ci, options.buildSystem)) {
            result.addWarning(fmt::format(
                    "CI system '{}' may require additional configuration with build system '{}'",
                    cli_enums::to_string(ci), cli_enums::to_string(options.buildSystem)));
        }
    }

    return result;
}

// Individual field validation
bool CliValidator::validateProjectName(const std::string& name, std::string& error) {
    if (name.empty()) {
        error = "Project name cannot be empty";
        return false;
    }

    if (name.length() > 100) {
        error = "Project name is too long (maximum 100 characters)";
        return false;
    }

    if (!isValidIdentifier(name)) {
        error = "Project name contains invalid characters (use only letters, numbers, hyphens, and "
                "underscores)";
        return false;
    }

    // Check for reserved names
    std::vector<std::string> reservedNames = {"con",  "prn",  "aux",  "nul",
                                              "com1", "com2", "lpt1", "lpt2"};
    std::string lowerName = StringUtils::toLower(name);
    if (std::find(reservedNames.begin(), reservedNames.end(), lowerName) != reservedNames.end()) {
        error = "Project name is a reserved system name";
        return false;
    }

    return true;
}

bool CliValidator::validateProjectPath(const std::string& path, std::string& error) {
    if (path.empty()) {
        error = "Project path cannot be empty";
        return false;
    }

    try {
        std::filesystem::path p(path);
        if (!std::filesystem::exists(p.parent_path())) {
            error = "Parent directory does not exist";
            return false;
        }
    } catch (const std::exception& e) {
        error = "Invalid path format: " + std::string(e.what());
        return false;
    }

    return true;
}

bool CliValidator::validateGitUrl(const std::string& url, std::string& error) {
    if (url.empty()) {
        error = "Git URL cannot be empty";
        return false;
    }

    // Basic URL validation
    std::regex urlPattern(R"(^(https?|git|ssh)://[^\s/$.?#].[^\s]*$|^git@[^\s:]+:[^\s]+\.git$)");
    if (!std::regex_match(url, urlPattern)) {
        error = "Invalid Git URL format";
        return false;
    }

    return true;
}

bool CliValidator::validateEmail(const std::string& email, std::string& error) {
    if (email.empty()) {
        error = "Email cannot be empty";
        return false;
    }

    if (!isValidEmailFormat(email)) {
        error = "Invalid email format";
        return false;
    }

    return true;
}

// Helper validation functions
bool CliValidator::isValidIdentifier(const std::string& name) {
    if (name.empty())
        return false;

    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }

    // Cannot start with a number or hyphen
    return std::isalpha(name[0]) || name[0] == '_';
}

bool CliValidator::isValidEmailFormat(const std::string& email) {
    std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, emailPattern);
}

// Compatibility checkers
bool CliValidator::isBuildSystemCompatible(BuildSystem build, PackageManager pkg) {
    // CMake works well with all package managers
    if (build == BuildSystem::CMake)
        return true;

    // Meson has good Conan integration
    if (build == BuildSystem::Meson && pkg == PackageManager::Conan)
        return true;

    // Bazel works best with its own package management
    if (build == BuildSystem::Bazel && pkg == PackageManager::None)
        return true;

    // Other combinations may work but with limitations
    return true;  // Allow but may generate warnings
}

bool CliValidator::isTestFrameworkCompatible(TestFramework test, BuildSystem build) {
    // All test frameworks work with CMake
    if (build == BuildSystem::CMake)
        return true;

    // Most test frameworks work with Meson
    if (build == BuildSystem::Meson && test != TestFramework::Boost)
        return true;

    // Bazel has its own testing approach
    if (build == BuildSystem::Bazel)
        return test == TestFramework::GTest;

    return true;  // Allow but may need additional configuration
}

bool CliValidator::isCiSystemCompatible(CiSystem ci, BuildSystem build) {
    (void)build;  // TODO: Implement build system specific CI compatibility checks
    // GitHub Actions and GitLab CI work with all build systems
    if (ci == CiSystem::GitHub || ci == CiSystem::GitLab)
        return true;

    // Other CI systems may need specific configuration
    return true;
}

// Compatibility validation
ValidationResult CliValidator::validateCompatibility(const CliOptions& options) {
    ValidationResult result;

    // Check overall compatibility between major components
    if (options.templateType == TemplateType::Embedded) {
        if (options.packageManager != PackageManager::None) {
            result.addWarning("Embedded projects typically don't use package managers");
        }
        if (options.includeTests) {
            result.addWarning("Testing frameworks may not be suitable for embedded projects");
        }
    }

    return result;
}

// Template-specific validation
ValidationResult CliValidator::validateTemplateRequirements(const CliOptions& options) {
    ValidationResult result;

    switch (options.templateType) {
        case TemplateType::Console:
            result.merge(validateConsoleTemplate(options));
            break;
        case TemplateType::Lib:
        case TemplateType::HeaderOnlyLib:
            result.merge(validateLibraryTemplate(options));
            break;
        case TemplateType::Gui:
        case TemplateType::QtApp:
            result.merge(validateGuiTemplate(options));
            break;
        case TemplateType::Network:
            result.merge(validateNetworkTemplate(options));
            break;
        case TemplateType::GameEngine:
        case TemplateType::SfmlApp:
            result.merge(validateGameTemplate(options));
            break;
        default:
            break;
    }

    return result;
}

// Configuration consistency validation
ValidationResult CliValidator::validateConsistency(const CliOptions& options) {
    ValidationResult result;

    // Check for conflicting options
    if (!options.initGit && !options.ciOptions.empty()) {
        result.addWarning("CI/CD systems are configured but Git is not initialized");
    }

    if (!options.includeDocumentation && options.generateDoxygen) {
        result.addWarning("Doxygen is enabled but documentation is not included");
    }

    if (options.includeShaders && options.templateType != TemplateType::Gui &&
        options.templateType != TemplateType::GameEngine) {
        result.addWarning("Shaders are included but template type doesn't typically use them");
    }

    return result;
}

// Template-specific validators
ValidationResult CliValidator::validateConsoleTemplate(const CliOptions& options) {
    ValidationResult result;

    if (!options.guiFrameworks.empty()) {
        result.addWarning("GUI frameworks specified for console application");
    }

    return result;
}

ValidationResult CliValidator::validateLibraryTemplate(const CliOptions& options) {
    ValidationResult result;

    if (options.templateType == TemplateType::HeaderOnlyLib &&
        options.buildSystem == BuildSystem::Make) {
        result.addWarning("Header-only libraries typically don't need complex build systems");
    }

    return result;
}

ValidationResult CliValidator::validateGuiTemplate(const CliOptions& options) {
    ValidationResult result;

    if (options.guiFrameworks.empty()) {
        result.addError("GUI template requires at least one GUI framework to be specified");
    }

    return result;
}

ValidationResult CliValidator::validateNetworkTemplate(const CliOptions& options) {
    ValidationResult result;

    if (!options.networkLibrary.has_value()) {
        result.addError("Network template requires a network library to be specified");
    }

    return result;
}

ValidationResult CliValidator::validateGameTemplate(const CliOptions& options) {
    ValidationResult result;

    if (options.gameFrameworks.empty() && options.graphicsLibraries.empty()) {
        result.addWarning("Game template should specify game frameworks or graphics libraries");
    }

    return result;
}

// Validation utilities implementation
namespace validation_utils {

void printValidationResult(const ValidationResult& result) {
    if (result.hasErrors()) {
        std::cout << TerminalUtils::colorize("❌ Validation Errors:", utils::Color::BrightRed)
                  << "\n";
        for (const auto& error : result.errors) {
            std::cout << TerminalUtils::colorize("  • " + error, utils::Color::Red) << "\n";
        }
        std::cout << "\n";
    }

    if (result.hasWarnings()) {
        std::cout << TerminalUtils::colorize("⚠️  Validation Warnings:", utils::Color::BrightYellow)
                  << "\n";
        for (const auto& warning : result.warnings) {
            std::cout << TerminalUtils::colorize("  • " + warning, utils::Color::Yellow) << "\n";
        }
        std::cout << "\n";
    }

    if (result.isValid && !result.hasWarnings()) {
        std::cout << TerminalUtils::colorize("✅ Configuration is valid!",
                                             utils::Color::BrightGreen)
                  << "\n\n";
    }
}

void printValidationSummary(const ValidationResult& result) {
    if (result.isValid) {
        std::string message = "✅ Valid";
        if (result.hasWarnings()) {
            message += fmt::format(" ({} warnings)", result.warnings.size());
        }
        std::cout << TerminalUtils::colorize(message, utils::Color::BrightGreen) << "\n";
    } else {
        std::string message = fmt::format("❌ Invalid ({} errors", result.errors.size());
        if (result.hasWarnings()) {
            message += fmt::format(", {} warnings", result.warnings.size());
        }
        message += ")";
        std::cout << TerminalUtils::colorize(message, utils::Color::BrightRed) << "\n";
    }
}

std::string formatValidationErrors(const ValidationResult& result) {
    std::string formatted;
    for (const auto& error : result.errors) {
        formatted += "❌ " + error + "\n";
    }
    return formatted;
}

std::string formatValidationWarnings(const ValidationResult& result) {
    std::string formatted;
    for (const auto& warning : result.warnings) {
        formatted += "⚠️  " + warning + "\n";
    }
    return formatted;
}

bool askUserToFixErrors(const ValidationResult& result) {
    if (!result.hasErrors())
        return false;

    printValidationResult(result);
    return UserInput::readConfirmation("Would you like to fix these configuration errors?", true);
}

CliOptions promptUserToFixConfiguration(const CliOptions& options, const ValidationResult& result) {
    CliOptions fixedOptions = options;

    // This would implement interactive fixing of configuration errors
    // For now, just return the original options
    std::cout << TerminalUtils::colorize("Interactive configuration fixing not yet implemented.",
                                         utils::Color::BrightYellow)
              << "\n";
    std::cout << "Please review the " << result.errors.size()
              << " errors above and adjust your configuration manually.\n\n";

    return fixedOptions;
}

}  // namespace validation_utils
