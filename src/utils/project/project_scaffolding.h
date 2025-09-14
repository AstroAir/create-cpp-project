#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "../../cli/cli_parser.h"

namespace utils {

// Project layout types
enum class ProjectLayout {
    Minimal,     // Basic structure with minimal directories
    Standard,    // Standard C++ project layout
    Enterprise,  // Enterprise-grade structure with extensive organization
    Monorepo,    // Monorepo structure for multiple projects
    HeaderOnly,  // Header-only library structure
    Modern,      // Modern C++ project with latest best practices
    Custom       // User-defined custom layout
};

// Directory structure definition
struct DirectoryStructure {
    std::string path;
    std::string description;
    bool required = true;
    bool createGitkeep = false;  // Create .gitkeep for empty directories
    std::vector<DirectoryStructure> subdirectories;
    std::vector<std::string> defaultFiles;  // Files to create by default
};

// File template definition
struct FileTemplate {
    std::string filename;
    std::string content;
    std::string description;
    bool executable = false;
    std::map<std::string, std::string> placeholders;  // For template substitution
};

// Best practices configuration
struct BestPracticesConfig {
    bool enableClangFormat = true;
    bool enableClangTidy = true;
    bool enableCppcheck = true;
    bool enableSanitizers = true;
    bool enableCodeCoverage = true;
    bool enableDocumentation = true;
    bool enableContinuousIntegration = true;
    bool enablePreCommitHooks = true;
    bool enableEditorConfig = true;
    bool enableGitignore = true;
    bool enableLicense = true;
    bool enableChangelog = true;
    bool enableContributing = true;
    bool enableCodeOfConduct = true;
    bool enableSecurityPolicy = true;
    bool enableIssueTemplates = true;
    bool enablePullRequestTemplate = true;
};

// Project scaffolding configuration
struct ScaffoldingConfig {
    ProjectLayout layout;
    DirectoryStructure rootStructure;
    std::vector<FileTemplate> fileTemplates;
    BestPracticesConfig bestPractices;
    std::map<std::string, std::string> globalPlaceholders;

    // Advanced options
    bool createExamples = true;
    bool createBenchmarks = false;
    bool createTools = false;
    bool createScripts = true;
    bool createDocs = true;
    bool createTests = true;
    bool createAssets = false;
    bool createConfigs = true;

    // Language and framework specific
    std::vector<std::string> additionalLanguages;  // Python scripts, shell scripts, etc.
    std::vector<std::string> frameworks;
    std::vector<std::string> tools;
};

// Project scaffolding manager
class ProjectScaffolding {
   public:
    static ProjectScaffolding& getInstance();

    // Main scaffolding operations
    bool createProjectStructure(const CliOptions& options, const ScaffoldingConfig& config);
    bool createProjectStructure(const CliOptions& options,
                                ProjectLayout layout = ProjectLayout::Standard);

    // Configuration management
    ScaffoldingConfig createDefaultConfig(TemplateType templateType, ProjectLayout layout);
    ScaffoldingConfig configureInteractively(const CliOptions& options);

    // Layout management
    DirectoryStructure getLayoutStructure(ProjectLayout layout, TemplateType templateType);
    std::vector<FileTemplate> getLayoutTemplates(ProjectLayout layout, TemplateType templateType);

    // Interactive configuration
    ProjectLayout selectProjectLayout(ProjectLayout defaultLayout = ProjectLayout::Standard);
    BestPracticesConfig configureBestPractices(const BestPracticesConfig& defaults);

    // Directory structure operations
    bool createDirectoryStructure(const std::string& basePath, const DirectoryStructure& structure);
    bool createFileFromTemplate(const std::string& filePath, const FileTemplate& fileTemplate,
                                const std::map<std::string, std::string>& placeholders);

    // Best practices setup
    bool setupBestPractices(const std::string& projectPath, const BestPracticesConfig& config,
                            const CliOptions& options);
    bool setupClangFormat(const std::string& projectPath);
    bool setupClangTidy(const std::string& projectPath);
    bool setupCppcheck(const std::string& projectPath);
    bool setupEditorConfig(const std::string& projectPath);
    bool setupGitHooks(const std::string& projectPath);
    bool setupCITemplates(const std::string& projectPath, const CliOptions& options);
    bool setupDocumentationTemplates(const std::string& projectPath, const CliOptions& options);
    bool setupIssueTemplates(const std::string& projectPath);
    bool setupSecurityPolicy(const std::string& projectPath);
    bool setupCodeOfConduct(const std::string& projectPath);
    bool setupContributing(const std::string& projectPath);
    bool setupChangelog(const std::string& projectPath);

    // Template management
    std::vector<FileTemplate> getCommonFileTemplates();
    std::vector<FileTemplate> getTemplateSpecificFiles(TemplateType templateType);
    std::vector<FileTemplate> getBestPracticeFiles(const BestPracticesConfig& config);

    // Validation and analysis
    bool validateProjectStructure(const std::string& projectPath, const ScaffoldingConfig& config);
    std::vector<std::string> analyzeProjectStructure(const std::string& projectPath);
    std::vector<std::string> suggestImprovements(const std::string& projectPath);

    // Import/Export functionality
    bool exportScaffoldingConfig(const ScaffoldingConfig& config, const std::string& filePath);
    std::optional<ScaffoldingConfig> importScaffoldingConfig(const std::string& filePath);

    // Preset management
    std::map<std::string, ScaffoldingConfig> getPresetConfigs();
    bool savePresetConfig(const std::string& name, const ScaffoldingConfig& config);
    bool loadPresetConfig(const std::string& name, ScaffoldingConfig& config);

    // Project migration and updates
    bool migrateProjectStructure(const std::string& projectPath, ProjectLayout fromLayout,
                                 ProjectLayout toLayout);
    bool updateProjectStructure(const std::string& projectPath, const ScaffoldingConfig& newConfig);

   private:
    ProjectScaffolding() = default;

    // Internal structure definitions
    void initializeLayoutStructures();
    void initializeFileTemplates();
    void initializeBestPracticesTemplates();

    // Layout structure definitions
    DirectoryStructure createMinimalLayout();
    DirectoryStructure createStandardLayout();
    DirectoryStructure createEnterpriseLayout();
    DirectoryStructure createMonorepoLayout();
    DirectoryStructure createHeaderOnlyLayout();
    DirectoryStructure createModernLayout();
    DirectoryStructure createMSYS2Layout();

    // Template content generators
    std::string generateReadmeContent(const CliOptions& options);
    std::string generateCMakeListsContent(const CliOptions& options);
    std::string generateGitignoreContent(TemplateType templateType);
    std::string generateClangFormatContent();
    std::string generateClangTidyContent();
    std::string generateEditorConfigContent();
    std::string generateLicenseContent(const std::string& license, const std::string& author);
    std::string generateContributingContent(const CliOptions& options);
    std::string generateCodeOfConductContent();
    std::string generateSecurityPolicyContent();
    std::string generateChangelogContent(const CliOptions& options);
    std::string generateDoxyfileContent(const CliOptions& options);
    std::string generatePreCommitHookContent();
    std::string generateCIWorkflowContent(const CliOptions& options);

    // Utility methods
    std::string processTemplate(const std::string& templateContent,
                                const std::map<std::string, std::string>& placeholders);
    std::map<std::string, std::string> createGlobalPlaceholders(const CliOptions& options);
    bool createGitkeepFile(const std::string& directoryPath);

    // Internal data
    std::map<ProjectLayout, DirectoryStructure> layoutStructures_;
    std::map<ProjectLayout, std::vector<FileTemplate>> layoutTemplates_;
    std::map<std::string, FileTemplate> commonTemplates_;
    std::map<std::string, ScaffoldingConfig> presetConfigs_;
};

// Utility functions for project scaffolding
namespace scaffolding_utils {
// String conversion utilities
std::string toString(ProjectLayout layout);
ProjectLayout parseProjectLayout(const std::string& str);
bool isValidProjectLayout(const std::string& str);

// Directory utilities
std::vector<std::string> getAllDirectories(const DirectoryStructure& structure);
std::vector<std::string> getAllFiles(const DirectoryStructure& structure);
bool isDirectoryEmpty(const std::string& path);

// Template utilities
std::vector<std::string> extractPlaceholders(const std::string& content);
bool validateTemplate(const FileTemplate& fileTemplate);

// Display utilities
void printDirectoryStructure(const DirectoryStructure& structure, int indent = 0);
void printScaffoldingConfig(const ScaffoldingConfig& config);
void printBestPracticesConfig(const BestPracticesConfig& config);

// Analysis utilities
std::map<std::string, int> analyzeDirectoryStructure(const std::string& projectPath);
std::vector<std::string> findMissingBestPractices(const std::string& projectPath);
std::vector<std::string> findUnusedDirectories(const std::string& projectPath);
}  // namespace scaffolding_utils

}  // namespace utils
