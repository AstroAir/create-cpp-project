#include "project_scaffolding.h"
#include "file_utils.h"
#include "terminal_utils.h"
#include "../cli/types/cli_enums.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <sstream>
#include <regex>

namespace utils {

ProjectScaffolding& ProjectScaffolding::getInstance() {
    static ProjectScaffolding instance;
    static bool initialized = false;

    if (!initialized) {
        instance.initializeLayoutStructures();
        instance.initializeFileTemplates();
        instance.initializeBestPracticesTemplates();
        initialized = true;
    }

    return instance;
}

bool ProjectScaffolding::createProjectStructure(const CliOptions& options, const ScaffoldingConfig& config) {
    std::string projectPath = options.projectName;

    TerminalUtils::showNpmStyleProgress("Creating project structure", 0, "Initializing");

    // Create root directory
    if (!FileUtils::createDirectory(projectPath)) {
        TerminalUtils::showNpmStyleError("Failed to create project directory", projectPath);
        return false;
    }

    TerminalUtils::showNpmStyleProgress("Creating project structure", 20, "Creating directories");

    // Create directory structure
    if (!createDirectoryStructure(projectPath, config.rootStructure)) {
        TerminalUtils::showNpmStyleError("Failed to create directory structure");
        return false;
    }

    TerminalUtils::showNpmStyleProgress("Creating project structure", 40, "Creating files");

    // Create files from templates
    auto globalPlaceholders = createGlobalPlaceholders(options);
    for (const auto& fileTemplate : config.fileTemplates) {
        std::string filePath = FileUtils::combinePath(projectPath, fileTemplate.filename);
        if (!createFileFromTemplate(filePath, fileTemplate, globalPlaceholders)) {
            TerminalUtils::showNpmStyleWarning("Failed to create file", fileTemplate.filename);
        }
    }

    TerminalUtils::showNpmStyleProgress("Creating project structure", 60, "Setting up best practices");

    // Setup best practices
    if (!setupBestPractices(projectPath, config.bestPractices, options)) {
        TerminalUtils::showNpmStyleWarning("Some best practices setup failed");
    }

    TerminalUtils::showNpmStyleProgress("Creating project structure", 80, "Finalizing");

    // Create additional template-specific files
    auto templateFiles = getTemplateSpecificFiles(options.templateType);
    for (const auto& fileTemplate : templateFiles) {
        std::string filePath = FileUtils::combinePath(projectPath, fileTemplate.filename);
        if (!createFileFromTemplate(filePath, fileTemplate, globalPlaceholders)) {
            TerminalUtils::showNpmStyleWarning("Failed to create template file", fileTemplate.filename);
        }
    }

    TerminalUtils::showNpmStyleProgress("Creating project structure", 100, "Complete");
    TerminalUtils::showNpmStyleSuccess("Project structure created successfully", projectPath);

    return true;
}

bool ProjectScaffolding::createProjectStructure(const CliOptions& options, ProjectLayout layout) {
    ScaffoldingConfig config = createDefaultConfig(options.templateType, layout);
    return createProjectStructure(options, config);
}

ScaffoldingConfig ProjectScaffolding::createDefaultConfig(TemplateType templateType, ProjectLayout layout) {
    ScaffoldingConfig config;
    config.layout = layout;
    config.rootStructure = getLayoutStructure(layout, templateType);
    config.fileTemplates = getLayoutTemplates(layout, templateType);

    // Set default best practices based on template type
    config.bestPractices.enableClangFormat = true;
    config.bestPractices.enableClangTidy = true;
    config.bestPractices.enableEditorConfig = true;
    config.bestPractices.enableGitignore = true;
    config.bestPractices.enableLicense = true;
    config.bestPractices.enableChangelog = true;

    switch (templateType) {
        case TemplateType::Lib:
        case TemplateType::HeaderOnlyLib:
            config.bestPractices.enableDocumentation = true;
            config.bestPractices.enableContributing = true;
            config.bestPractices.enableCodeOfConduct = true;
            config.createExamples = true;
            config.createBenchmarks = true;
            break;

        case TemplateType::GameEngine:
            config.bestPractices.enableSanitizers = true;
            config.bestPractices.enableCodeCoverage = true;
            config.createAssets = true;
            config.createTools = true;
            break;

        case TemplateType::Embedded:
            config.bestPractices.enableCppcheck = true;
            config.createTools = true;
            break;

        default:
            // Use defaults
            break;
    }

    return config;
}

ScaffoldingConfig ProjectScaffolding::configureInteractively(const CliOptions& options) {
    TerminalUtils::showNpmStyleHeader("Project Scaffolding Configuration");

    // Select project layout
    ProjectLayout layout = selectProjectLayout();

    // Create base config
    ScaffoldingConfig config = createDefaultConfig(options.templateType, layout);

    // Configure best practices
    config.bestPractices = configureBestPractices(config.bestPractices);

    // Additional options
    std::vector<std::string> additionalOptions = {
        "Create examples directory",
        "Create benchmarks directory",
        "Create tools directory",
        "Create assets directory",
        "Create comprehensive documentation"
    };

    std::vector<bool> defaultSelected = {
        config.createExamples,
        config.createBenchmarks,
        config.createTools,
        config.createAssets,
        config.createDocs
    };

    auto selectedOptions = TerminalUtils::showMultiSelectDialog(
        "Select additional project features", additionalOptions, defaultSelected);

    // Apply selections
    config.createExamples = std::find(selectedOptions.begin(), selectedOptions.end(),
                                     "Create examples directory") != selectedOptions.end();
    config.createBenchmarks = std::find(selectedOptions.begin(), selectedOptions.end(),
                                       "Create benchmarks directory") != selectedOptions.end();
    config.createTools = std::find(selectedOptions.begin(), selectedOptions.end(),
                                  "Create tools directory") != selectedOptions.end();
    config.createAssets = std::find(selectedOptions.begin(), selectedOptions.end(),
                                   "Create assets directory") != selectedOptions.end();
    config.createDocs = std::find(selectedOptions.begin(), selectedOptions.end(),
                                 "Create comprehensive documentation") != selectedOptions.end();

    TerminalUtils::showNpmStyleSuccess("Scaffolding configuration completed");

    return config;
}

ProjectLayout ProjectScaffolding::selectProjectLayout(ProjectLayout defaultLayout) {
    std::vector<std::string> layouts = {
        "Minimal - Basic structure with essential directories only",
        "Standard - Traditional C++ project layout",
        "Enterprise - Comprehensive structure for large projects",
        "Monorepo - Multi-project repository structure",
        "Header-Only - Optimized for header-only libraries",
        "Modern - Latest C++ best practices and tooling"
    };

    int defaultIndex = static_cast<int>(defaultLayout);

    int selected = TerminalUtils::showInteractiveMenu(
        layouts, "Select project layout", defaultIndex);

    if (selected >= 0 && selected < static_cast<int>(layouts.size())) {
        std::string layoutName = layouts[selected].substr(0, layouts[selected].find(' '));
        TerminalUtils::showNpmStyleSuccess("Project layout selected", layoutName);
        return static_cast<ProjectLayout>(selected);
    }

    return defaultLayout;
}

BestPracticesConfig ProjectScaffolding::configureBestPractices(const BestPracticesConfig& defaults) {
    std::vector<std::string> practices = {
        "Enable clang-format (code formatting)",
        "Enable clang-tidy (static analysis)",
        "Enable cppcheck (additional static analysis)",
        "Enable sanitizers (runtime checks)",
        "Enable code coverage reporting",
        "Enable comprehensive documentation",
        "Enable CI/CD workflows",
        "Enable pre-commit hooks",
        "Enable editor configuration",
        "Enable issue templates",
        "Enable pull request template",
        "Enable security policy",
        "Enable code of conduct",
        "Enable contributing guidelines"
    };

    std::vector<bool> defaultSelected = {
        defaults.enableClangFormat,
        defaults.enableClangTidy,
        defaults.enableCppcheck,
        defaults.enableSanitizers,
        defaults.enableCodeCoverage,
        defaults.enableDocumentation,
        defaults.enableContinuousIntegration,
        defaults.enablePreCommitHooks,
        defaults.enableEditorConfig,
        defaults.enableIssueTemplates,
        defaults.enablePullRequestTemplate,
        defaults.enableSecurityPolicy,
        defaults.enableCodeOfConduct,
        defaults.enableContributing
    };

    auto selectedPractices = TerminalUtils::showMultiSelectDialog(
        "Select best practices to enable", practices, defaultSelected);

    BestPracticesConfig config = defaults;

    // Apply selections
    config.enableClangFormat = std::find(selectedPractices.begin(), selectedPractices.end(),
                                        "Enable clang-format (code formatting)") != selectedPractices.end();
    config.enableClangTidy = std::find(selectedPractices.begin(), selectedPractices.end(),
                                      "Enable clang-tidy (static analysis)") != selectedPractices.end();
    config.enableCppcheck = std::find(selectedPractices.begin(), selectedPractices.end(),
                                     "Enable cppcheck (additional static analysis)") != selectedPractices.end();
    config.enableSanitizers = std::find(selectedPractices.begin(), selectedPractices.end(),
                                       "Enable sanitizers (runtime checks)") != selectedPractices.end();
    config.enableCodeCoverage = std::find(selectedPractices.begin(), selectedPractices.end(),
                                         "Enable code coverage reporting") != selectedPractices.end();
    config.enableDocumentation = std::find(selectedPractices.begin(), selectedPractices.end(),
                                          "Enable comprehensive documentation") != selectedPractices.end();
    config.enableContinuousIntegration = std::find(selectedPractices.begin(), selectedPractices.end(),
                                                   "Enable CI/CD workflows") != selectedPractices.end();
    config.enablePreCommitHooks = std::find(selectedPractices.begin(), selectedPractices.end(),
                                           "Enable pre-commit hooks") != selectedPractices.end();
    config.enableEditorConfig = std::find(selectedPractices.begin(), selectedPractices.end(),
                                         "Enable editor configuration") != selectedPractices.end();
    config.enableIssueTemplates = std::find(selectedPractices.begin(), selectedPractices.end(),
                                           "Enable issue templates") != selectedPractices.end();
    config.enablePullRequestTemplate = std::find(selectedPractices.begin(), selectedPractices.end(),
                                                 "Enable pull request template") != selectedPractices.end();
    config.enableSecurityPolicy = std::find(selectedPractices.begin(), selectedPractices.end(),
                                           "Enable security policy") != selectedPractices.end();
    config.enableCodeOfConduct = std::find(selectedPractices.begin(), selectedPractices.end(),
                                          "Enable code of conduct") != selectedPractices.end();
    config.enableContributing = std::find(selectedPractices.begin(), selectedPractices.end(),
                                         "Enable contributing guidelines") != selectedPractices.end();

    return config;
}

bool ProjectScaffolding::createDirectoryStructure(const std::string& basePath, const DirectoryStructure& structure) {
    std::string fullPath = FileUtils::combinePath(basePath, structure.path);

    // Create the directory
    if (!FileUtils::createDirectory(fullPath)) {
        spdlog::error("Failed to create directory: {}", fullPath);
        return false;
    }

    // Create .gitkeep if needed
    if (structure.createGitkeep) {
        createGitkeepFile(fullPath);
    }

    // Create default files
    for (const auto& filename : structure.defaultFiles) {
        std::string filePath = FileUtils::combinePath(fullPath, filename);
        if (!FileUtils::writeToFile(filePath, "")) {
            spdlog::warn("Failed to create default file: {}", filePath);
        }
    }

    // Recursively create subdirectories
    for (const auto& subdir : structure.subdirectories) {
        if (!createDirectoryStructure(fullPath, subdir)) {
            return false;
        }
    }

    return true;
}

bool ProjectScaffolding::createFileFromTemplate(const std::string& filePath, const FileTemplate& fileTemplate,
                                               const std::map<std::string, std::string>& placeholders) {
    // Process template content
    std::string content = processTemplate(fileTemplate.content, placeholders);

    // Create directory if it doesn't exist
    std::string directory = FileUtils::getDirectoryName(filePath);
    if (!FileUtils::directoryExists(directory)) {
        if (!FileUtils::createDirectory(directory)) {
            spdlog::error("Failed to create directory for file: {}", directory);
            return false;
        }
    }

    // Write file
    if (!FileUtils::writeToFile(filePath, content)) {
        spdlog::error("Failed to write file: {}", filePath);
        return false;
    }

    // Set executable if needed
    if (fileTemplate.executable) {
        if (!FileUtils::setExecutable(filePath)) {
            spdlog::warn("Failed to set executable permission for: {}", filePath);
        }
    }

    return true;
}

void ProjectScaffolding::initializeLayoutStructures() {
    layoutStructures_[ProjectLayout::Minimal] = createMinimalLayout();
    layoutStructures_[ProjectLayout::Standard] = createStandardLayout();
    layoutStructures_[ProjectLayout::Enterprise] = createEnterpriseLayout();
    layoutStructures_[ProjectLayout::Monorepo] = createMonorepoLayout();
    layoutStructures_[ProjectLayout::HeaderOnly] = createHeaderOnlyLayout();
    layoutStructures_[ProjectLayout::Modern] = createModernLayout();
}

void ProjectScaffolding::initializeFileTemplates() {
    // Initialize common file templates
    // This would be expanded with actual template content
}

void ProjectScaffolding::initializeBestPracticesTemplates() {
    // Initialize best practices templates
    // This would be expanded with actual template content
}

DirectoryStructure ProjectScaffolding::createStandardLayout() {
    DirectoryStructure root;
    root.path = "";
    root.description = "Project root";

    // Source directory
    DirectoryStructure src;
    src.path = "src";
    src.description = "Source code";
    src.required = true;

    // Include directory
    DirectoryStructure include;
    include.path = "include";
    include.description = "Public headers";
    include.required = true;

    // Tests directory
    DirectoryStructure tests;
    tests.path = "tests";
    tests.description = "Unit tests";
    tests.createGitkeep = true;

    // Documentation directory
    DirectoryStructure docs;
    docs.path = "docs";
    docs.description = "Documentation";
    docs.createGitkeep = true;

    // Build directory
    DirectoryStructure build;
    build.path = "build";
    build.description = "Build artifacts";
    build.createGitkeep = true;

    // Scripts directory
    DirectoryStructure scripts;
    scripts.path = "scripts";
    scripts.description = "Build and utility scripts";
    scripts.createGitkeep = true;

    root.subdirectories = {src, include, tests, docs, build, scripts};

    return root;
}

std::map<std::string, std::string> ProjectScaffolding::createGlobalPlaceholders(const CliOptions& options) {
    std::map<std::string, std::string> placeholders;

    placeholders["PROJECT_NAME"] = options.projectName;
    placeholders["PROJECT_DESCRIPTION"] = "A C++ project created with cpp-scaffold";
    placeholders["AUTHOR_NAME"] = options.gitUserName.empty() ? "Project Author" : options.gitUserName;
    placeholders["AUTHOR_EMAIL"] = options.gitUserEmail.empty() ? "author@example.com" : options.gitUserEmail;
    placeholders["CURRENT_YEAR"] = "2024"; // Would be dynamic in real implementation
    placeholders["CPP_STANDARD"] = "17"; // Would be configurable
    placeholders["BUILD_SYSTEM"] = std::string(cli_enums::to_string(options.buildSystem));
    placeholders["PACKAGE_MANAGER"] = std::string(cli_enums::to_string(options.packageManager));

    return placeholders;
}

std::string ProjectScaffolding::processTemplate(const std::string& templateContent,
                                               const std::map<std::string, std::string>& placeholders) {
    std::string result = templateContent;

    for (const auto& [placeholder, value] : placeholders) {
        std::string pattern = "{{" + placeholder + "}}";
        size_t pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.replace(pos, pattern.length(), value);
            pos += value.length();
        }
    }

    return result;
}

bool ProjectScaffolding::createGitkeepFile(const std::string& directoryPath) {
    std::string gitkeepPath = FileUtils::combinePath(directoryPath, ".gitkeep");
    return FileUtils::writeToFile(gitkeepPath, "");
}

} // namespace utils
