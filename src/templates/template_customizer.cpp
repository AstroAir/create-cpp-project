#include "template_customizer.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>

#include "../cli/input/user_input.h"
#include "../cli/types/cli_enums.h"
#include "../utils/core/file_utils.h"
#include "../utils/core/string_utils.h"
#include "../utils/ui/terminal_utils.h"

using namespace utils;
using json = nlohmann::json;

namespace templates {

// Static member initialization
std::vector<TemplateCustomization> TemplateCustomizer::s_registeredTemplates;
bool TemplateCustomizer::s_initialized = false;

TemplateCustomization TemplateCustomizer::customizeTemplate(TemplateType templateType,
                                                            const CliOptions& options) {
    if (!s_initialized) {
        initializeDefaultTemplates();
    }

    // Get base template
    const TemplateCustomization* baseTemplate = getTemplate(templateType);
    if (!baseTemplate) {
        spdlog::error("Template not found for type: {}", static_cast<int>(templateType));
        return {};
    }

    TemplateCustomization customization = *baseTemplate;

    // Apply options-based customization
    customization.placeholders = generatePlaceholders(options);

    // Customize directory structure based on project structure preference
    if (options.projectStructure == ProjectStructure::Minimal) {
        customization.structure = getMinimalStructure(templateType);
    } else if (options.projectStructure == ProjectStructure::Advanced) {
        customization.structure = getAdvancedStructure(templateType);
    } else if (options.projectStructure == ProjectStructure::Custom) {
        // Use custom directories from options
        if (options.additionalOptions.count("custom_directories")) {
            customization.structure =
                    parseCustomStructure(options.additionalOptions.at("custom_directories"),
                                         options.additionalOptions.count("custom_files")
                                                 ? options.additionalOptions.at("custom_files")
                                                 : "");
        }
    }

    // Select features based on options
    std::vector<std::string> enabledFeatures = customization.defaultFeatures;

    if (options.includeTests) {
        enabledFeatures.push_back("testing");
        enabledFeatures.push_back("test-" +
                                  std::string(cli_enums::to_string(options.testFramework)));
    }

    if (options.includeDocumentation) {
        enabledFeatures.push_back("documentation");
        if (options.generateDoxygen) {
            enabledFeatures.push_back("doxygen");
        }
    }

    if (options.includeCodeStyleTools) {
        enabledFeatures.push_back("code-style");
        enabledFeatures.push_back("clang-format");
        enabledFeatures.push_back("clang-tidy");
    }

    // Add CI/CD features
    for (const auto& ci : options.ciOptions) {
        enabledFeatures.push_back("ci-" + std::string(cli_enums::to_string(ci)));
    }

    // Add editor configurations
    for (const auto& editor : options.editorOptions) {
        enabledFeatures.push_back("editor-" + std::string(cli_enums::to_string(editor)));
    }

    // Select template files based on enabled features
    customization.structure.files =
            selectTemplateFiles(customization.structure.files, enabledFeatures, options);

    return customization;
}

TemplateCustomization TemplateCustomizer::runInteractiveCustomization(
        TemplateType templateType, const CliOptions& baseOptions) {
    TerminalUtils::showInfo("🎨 Interactive Template Customization");
    TerminalUtils::showSectionSeparator();

    // Get base template
    const TemplateCustomization* baseTemplate = getTemplate(templateType);
    if (!baseTemplate) {
        TerminalUtils::showNpmStyleError("Template not found", "Invalid template type");
        return {};
    }

    TemplateCustomization customization = *baseTemplate;

    // Show template overview
    std::cout << TerminalUtils::colorize("📋 Template: ", Color::BrightCyan)
              << TerminalUtils::colorize(customization.templateName, Color::BrightWhite) << "\n";
    std::cout << TerminalUtils::colorize("📝 Description: ", Color::BrightBlack)
              << customization.description << "\n\n";

    // Feature selection
    std::cout << TerminalUtils::colorize("🔧 Feature Selection:", Color::BrightYellow) << "\n";
    std::vector<std::string> selectedFeatures =
            selectFeatures(customization.availableFeatures, customization.defaultFeatures,
                           customization.templateName);

    // Directory structure customization
    std::cout << "\n"
              << TerminalUtils::colorize("📁 Directory Structure:", Color::BrightMagenta) << "\n";
    customization.structure = customizeDirectoryStructure(customization.structure, baseOptions);

    // File selection
    std::cout << "\n" << TerminalUtils::colorize("📄 File Selection:", Color::BrightGreen) << "\n";
    customization.structure.files =
            selectTemplateFiles(customization.structure.files, selectedFeatures, baseOptions);

    // Generate placeholders
    customization.placeholders = generatePlaceholders(baseOptions);

    // Show preview and confirm
    showTemplatePreview(customization);

    if (!confirmCustomization(customization)) {
        TerminalUtils::showNpmStyleWarning("Customization cancelled", "Using default template");
        return *baseTemplate;
    }

    // Ask if user wants to save as custom template
    if (UserInput::readConfirmation("Save this customization as a reusable template?", false)) {
        std::string templateName;
        std::cout << "Enter template name: ";
        std::getline(std::cin, templateName);

        if (!templateName.empty()) {
            if (saveCustomTemplate(customization, templateName)) {
                TerminalUtils::showNpmStyleSuccess("Custom template saved: " + templateName);
            } else {
                TerminalUtils::showNpmStyleError("Failed to save custom template",
                                                 "Check permissions");
            }
        }
    }

    return customization;
}

std::vector<std::string> TemplateCustomizer::selectFeatures(
        const std::vector<std::string>& availableFeatures,
        const std::vector<std::string>& defaultFeatures, const std::string& templateName) {
    std::vector<std::string> selectedFeatures = defaultFeatures;

    std::cout << TerminalUtils::colorize("Available features for " + templateName + ":",
                                         Color::BrightCyan)
              << "\n";

    for (const auto& feature : availableFeatures) {
        bool isDefault = std::find(defaultFeatures.begin(), defaultFeatures.end(), feature) !=
                         defaultFeatures.end();
        bool isSelected = isDefault;

        std::string prompt = fmt::format("Include {} feature?", feature);
        if (isDefault) {
            prompt += " (recommended)";
        }

        showFeatureDetails(feature);

        isSelected = UserInput::readConfirmation(prompt, isDefault);

        if (isSelected && !isDefault) {
            selectedFeatures.push_back(feature);
        } else if (!isSelected && isDefault) {
            selectedFeatures.erase(
                    std::remove(selectedFeatures.begin(), selectedFeatures.end(), feature),
                    selectedFeatures.end());
        }
    }

    return selectedFeatures;
}

std::unordered_map<std::string, std::string> TemplateCustomizer::generatePlaceholders(
        const CliOptions& options) {
    std::unordered_map<std::string, std::string> placeholders;

    // Basic project information
    placeholders["PROJECT_NAME"] = options.projectName;
    placeholders["PROJECT_NAME_UPPER"] = StringUtils::toUpper(options.projectName);
    placeholders["PROJECT_NAME_LOWER"] = StringUtils::toLower(options.projectName);
    placeholders["PROJECT_DESCRIPTION"] = options.projectDescription.empty()
                                                  ? "A C++ project created with cpp-scaffold"
                                                  : options.projectDescription;

    // Replace hyphens with underscores for C++ identifiers
    std::string cppName = options.projectName;
    std::replace(cppName.begin(), cppName.end(), '-', '_');
    placeholders["PROJECT_NAME_CPP"] = cppName;
    placeholders["PROJECT_NAME_CPP_UPPER"] = StringUtils::toUpper(cppName);

    // Build system and configuration
    placeholders["BUILD_SYSTEM"] = std::string(cli_enums::to_string(options.buildSystem));
    placeholders["PACKAGE_MANAGER"] = std::string(cli_enums::to_string(options.packageManager));
    placeholders["CPP_STANDARD"] = std::string(cli_enums::to_string(options.cppStandard));

    // Extract numeric part of C++ standard
    std::string cppStdStr = std::string(cli_enums::to_string(options.cppStandard));
    if (cppStdStr.substr(0, 3) == "c++") {
        placeholders["CPP_STANDARD_NUM"] = cppStdStr.substr(3);
    } else {
        placeholders["CPP_STANDARD_NUM"] = "17";  // Default fallback
    }

    // Testing configuration
    if (options.includeTests) {
        placeholders["TEST_FRAMEWORK"] = std::string(cli_enums::to_string(options.testFramework));
        placeholders["ENABLE_TESTING"] = "ON";
    } else {
        placeholders["ENABLE_TESTING"] = "OFF";
    }

    // Documentation
    placeholders["ENABLE_DOCS"] = options.includeDocumentation ? "ON" : "OFF";
    placeholders["ENABLE_DOXYGEN"] = options.generateDoxygen ? "ON" : "OFF";

    // Date and author information
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);

    placeholders["CURRENT_YEAR"] = std::to_string(1900 + tm.tm_year);
    placeholders["CURRENT_DATE"] =
            fmt::format("{:04d}-{:02d}-{:02d}", 1900 + tm.tm_year, tm.tm_mon + 1, tm.tm_mday);

    // Author information (could be retrieved from git config or environment)
    placeholders["AUTHOR_NAME"] = "Your Name";
    placeholders["AUTHOR_EMAIL"] = "your.email@example.com";

    // License information
    if (options.licenseType != LicenseType::None) {
        placeholders["LICENSE_TYPE"] = std::string(cli_enums::to_string(options.licenseType));
        placeholders["ENABLE_LICENSE"] = "true";
    } else {
        placeholders["ENABLE_LICENSE"] = "false";
    }

    return placeholders;
}

std::string TemplateCustomizer::processTemplate(
        const std::string& templateContent,
        const std::unordered_map<std::string, std::string>& placeholders) {
    std::string result = templateContent;

    // Replace placeholders in the format {{PLACEHOLDER_NAME}}
    for (const auto& [key, value] : placeholders) {
        std::string placeholder = "{{" + key + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }

    // Handle conditional blocks: {{#IF CONDITION}}...{{/IF}}
    std::regex conditionalRegex(R"(\{\{#IF\s+(\w+)\}\}(.*?)\{\{/IF\}\})");
    std::smatch match;

    while (std::regex_search(result, match, conditionalRegex)) {
        std::string condition = match[1].str();
        std::string content = match[2].str();

        // Check if condition is true (placeholder exists and is not "false", "0", or "OFF")
        bool conditionTrue = false;
        if (placeholders.count(condition)) {
            std::string value = placeholders.at(condition);
            conditionTrue = !value.empty() && value != "false" && value != "0" && value != "OFF";
        }

        std::string replacement = conditionTrue ? content : "";
        result.replace(match.position(), match.length(), replacement);
    }

    return result;
}

void TemplateCustomizer::initializeDefaultTemplates() {
    s_registeredTemplates.clear();

    // Register predefined templates
    s_registeredTemplates.push_back(predefined::getConsoleTemplate());
    s_registeredTemplates.push_back(predefined::getLibraryTemplate());
    s_registeredTemplates.push_back(predefined::getHeaderOnlyLibTemplate());
    s_registeredTemplates.push_back(predefined::getGuiTemplate());
    s_registeredTemplates.push_back(predefined::getNetworkTemplate());
    s_registeredTemplates.push_back(predefined::getWebServiceTemplate());
    s_registeredTemplates.push_back(predefined::getEmbeddedTemplate());
    s_registeredTemplates.push_back(predefined::getGameEngineTemplate());

    // Load custom templates
    loadCustomTemplates();

    s_initialized = true;
}

const TemplateCustomization* TemplateCustomizer::getTemplate(TemplateType type) {
    if (!s_initialized) {
        initializeDefaultTemplates();
    }

    for (const auto& template_ : s_registeredTemplates) {
        // Compare by template name since we don't have a direct type field
        std::string typeName = std::string(cli_enums::to_string(type));
        if (template_.templateName == typeName) {
            return &template_;
        }
    }

    return nullptr;
}

DirectoryStructure TemplateCustomizer::getMinimalStructure(TemplateType type) {
    DirectoryStructure structure;
    structure.name = "minimal";
    structure.description = "Minimal project structure with essential directories only";

    switch (type) {
        case TemplateType::Console:
        case TemplateType::Gui:
        case TemplateType::Network:
        case TemplateType::WebService:
        case TemplateType::GameEngine:
            structure.directories = {"src", "include"};
            break;
        case TemplateType::Lib:
        case TemplateType::HeaderOnlyLib:
            structure.directories = {"include", "src"};
            break;
        default:
            structure.directories = {"src"};
            break;
    }

    return structure;
}

DirectoryStructure TemplateCustomizer::getAdvancedStructure(TemplateType type) {
    DirectoryStructure structure;
    structure.name = "advanced";
    structure.description = "Full-featured project structure with all recommended directories";

    switch (type) {
        case TemplateType::Console:
            structure.directories = {"src",      "include", "tests", "docs",
                                     "examples", "scripts", "cmake"};
            break;
        case TemplateType::Lib:
            structure.directories = {"include",  "src",        "tests", "docs",
                                     "examples", "benchmarks", "cmake", "tools"};
            break;
        case TemplateType::HeaderOnlyLib:
            structure.directories = {"include",  "tests", "docs",
                                     "examples", "cmake", "single_include"};
            break;
        case TemplateType::Gui:
            structure.directories = {"src",   "include", "resources", "assets",
                                     "tests", "docs",    "cmake"};
            break;
        case TemplateType::Network:
            structure.directories = {"src",  "include", "tests",  "examples",
                                     "docs", "cmake",   "configs"};
            break;
        case TemplateType::WebService:
            structure.directories = {"src",    "include", "tests",      "docs",
                                     "docker", "configs", "migrations", "cmake"};
            break;
        case TemplateType::GameEngine:
            structure.directories = {"src",      "include", "assets", "shaders", "tests",
                                     "examples", "docs",    "tools",  "cmake"};
            break;
        default:
            structure.directories = {"src", "include", "tests", "docs", "cmake"};
            break;
    }

    return structure;
}

DirectoryStructure TemplateCustomizer::parseCustomStructure(const std::string& directoriesStr,
                                                            const std::string& filesStr) {
    DirectoryStructure structure;
    structure.name = "custom";
    structure.description = "User-customized project structure";

    // Parse directories
    std::istringstream dirStream(directoriesStr);
    std::string dir;
    while (std::getline(dirStream, dir, ',')) {
        if (!dir.empty()) {
            structure.directories.push_back(dir);
        }
    }

    // Parse files (simplified - would need more complex parsing for full TemplateFile objects)
    std::istringstream fileStream(filesStr);
    std::string file;
    while (std::getline(fileStream, file, ',')) {
        if (!file.empty()) {
            TemplateFile templateFile;
            templateFile.relativePath = file;
            templateFile.description = "User-selected file";
            templateFile.isOptional = true;
            structure.files.push_back(templateFile);
        }
    }

    return structure;
}

std::vector<TemplateFile> TemplateCustomizer::selectTemplateFiles(
        const std::vector<TemplateFile>& availableFiles,
        const std::vector<std::string>& enabledFeatures, const CliOptions& options) {
    std::vector<TemplateFile> selectedFiles;

    for (const auto& file : availableFiles) {
        bool shouldInclude = true;

        // Check required features
        for (const auto& requiredFeature : file.requiredFeatures) {
            if (std::find(enabledFeatures.begin(), enabledFeatures.end(), requiredFeature) ==
                enabledFeatures.end()) {
                shouldInclude = false;
                break;
            }
        }

        // Check custom condition
        if (shouldInclude && file.shouldInclude) {
            shouldInclude = file.shouldInclude(options);
        }

        if (shouldInclude) {
            selectedFiles.push_back(file);
        }
    }

    return selectedFiles;
}

void TemplateCustomizer::showFeatureDetails(const std::string& feature) {
    // Feature descriptions
    static const std::unordered_map<std::string, std::string> featureDescriptions = {
            {"testing", "Adds unit testing framework and test directory structure"},
            {"documentation", "Includes README, API docs, and documentation generation"},
            {"doxygen", "Configures Doxygen for automatic API documentation"},
            {"code-style", "Adds code formatting and static analysis tools"},
            {"clang-format", "Configures clang-format for consistent code formatting"},
            {"clang-tidy", "Configures clang-tidy for static code analysis"},
            {"benchmarks", "Adds performance benchmarking framework and examples"},
            {"examples", "Includes example code and usage demonstrations"},
            {"docker", "Adds Docker configuration for containerized deployment"},
            {"ci-github", "Configures GitHub Actions for continuous integration"},
            {"ci-gitlab", "Configures GitLab CI/CD pipelines"},
            {"editor-vscode", "Adds VS Code configuration and settings"},
            {"editor-clion", "Adds CLion IDE configuration"}};

    if (featureDescriptions.count(feature)) {
        std::cout << "  "
                  << TerminalUtils::colorize("💡 " + featureDescriptions.at(feature),
                                             Color::BrightBlack)
                  << "\n";
    }
}

void TemplateCustomizer::showTemplatePreview(const TemplateCustomization& customization) {
    TerminalUtils::clearScreen();

    std::vector<std::string> previewLines = {"📋 Template Customization Preview",
                                             "Review your customized template before applying"};
    TerminalUtils::showBox(previewLines, BorderStyle::Double, Color::BrightGreen, Color::White, "");

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("Template: ", Color::BrightYellow)
              << TerminalUtils::colorize(customization.templateName, Color::BrightWhite) << "\n";
    std::cout << TerminalUtils::colorize("Description: ", Color::BrightBlack)
              << customization.description << "\n\n";

    std::cout << TerminalUtils::colorize("Directory Structure:", Color::BrightCyan) << "\n";
    for (const auto& dir : customization.structure.directories) {
        std::cout << "  📁 " << TerminalUtils::colorize(dir, Color::BrightWhite) << "\n";
    }

    std::cout << "\n"
              << TerminalUtils::colorize("Files to be created:", Color::BrightMagenta) << "\n";
    for (const auto& file : customization.structure.files) {
        std::string icon = file.isOptional ? "📄" : "📋";
        std::cout << "  " << icon << " "
                  << TerminalUtils::colorize(file.relativePath, Color::BrightWhite);
        if (!file.description.empty()) {
            std::cout << " - " << TerminalUtils::colorize(file.description, Color::BrightBlack);
        }
        std::cout << "\n";
    }

    std::cout << "\n";
}

bool TemplateCustomizer::confirmCustomization(const TemplateCustomization& customization) {
    (void)customization;  // Suppress unused parameter warning
    return UserInput::readConfirmation("Apply this template customization?", true);
}

void TemplateCustomizer::loadCustomTemplates() {
    // Implementation would load custom templates from user's config directory
    spdlog::debug("Loading custom templates from user configuration");
}

bool TemplateCustomizer::saveCustomTemplate(const TemplateCustomization& customization,
                                            const std::string& name) {
    (void)customization;  // Suppress unused parameter warning
    (void)name;           // Suppress unused parameter warning

    // Implementation would save custom template to user's config directory
    spdlog::info("Saving custom template: {}", name);
    return true;
}

}  // namespace templates
