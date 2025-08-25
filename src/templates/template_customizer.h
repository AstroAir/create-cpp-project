#pragma once
#include "../cli/cli_parser.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <functional>

namespace templates {

// Template file configuration
struct TemplateFile {
    std::string relativePath;
    std::string content;
    bool isTemplate = true; // If true, content will be processed for placeholders
    bool isOptional = false;
    std::string description;
    std::vector<std::string> requiredFeatures; // Features that must be enabled to include this file
    std::function<bool(const CliOptions&)> shouldInclude; // Custom condition function
};

// Directory structure configuration
struct DirectoryStructure {
    std::string name;
    std::string description;
    std::vector<std::string> directories;
    std::vector<TemplateFile> files;
    bool isDefault = false;
};

// Template customization options
struct TemplateCustomization {
    std::string templateName;
    std::string description;
    DirectoryStructure structure;
    std::vector<std::string> availableFeatures;
    std::vector<std::string> defaultFeatures;
    std::unordered_map<std::string, std::string> placeholders;
    std::vector<std::string> supportedBuildSystems;
    std::vector<std::string> recommendedPackages;
};

// Template customizer class
class TemplateCustomizer {
public:
    // Main customization interface
    static TemplateCustomization customizeTemplate(
        TemplateType templateType,
        const CliOptions& options
    );

    // Interactive template customization
    static TemplateCustomization runInteractiveCustomization(
        TemplateType templateType,
        const CliOptions& baseOptions
    );

    // Directory structure customization
    static DirectoryStructure customizeDirectoryStructure(
        const DirectoryStructure& baseStructure,
        const CliOptions& options
    );

    // File selection and customization
    static std::vector<TemplateFile> selectTemplateFiles(
        const std::vector<TemplateFile>& availableFiles,
        const std::vector<std::string>& enabledFeatures,
        const CliOptions& options
    );

    // Feature selection
    static std::vector<std::string> selectFeatures(
        const std::vector<std::string>& availableFeatures,
        const std::vector<std::string>& defaultFeatures,
        const std::string& templateName
    );

    // Template processing
    static std::string processTemplate(
        const std::string& templateContent,
        const std::unordered_map<std::string, std::string>& placeholders
    );

    // Placeholder generation
    static std::unordered_map<std::string, std::string> generatePlaceholders(
        const CliOptions& options
    );

    // Template validation
    static bool validateTemplateCustomization(
        const TemplateCustomization& customization
    );

    // Template registry
    static void registerTemplate(const TemplateCustomization& customization);
    static std::vector<TemplateCustomization> getAvailableTemplates();
    static const TemplateCustomization* getTemplate(TemplateType type);

    // Custom template management
    static bool saveCustomTemplate(
        const TemplateCustomization& customization,
        const std::string& name
    );
    static bool loadCustomTemplate(
        const std::string& name,
        TemplateCustomization& customization
    );
    static std::vector<std::string> listCustomTemplates();
    static bool deleteCustomTemplate(const std::string& name);

    // Template export/import
    static bool exportTemplate(
        const TemplateCustomization& customization,
        const std::filesystem::path& exportPath
    );
    static bool importTemplate(
        const std::filesystem::path& importPath,
        TemplateCustomization& customization
    );

private:
    // Internal helper methods
    static DirectoryStructure getDefaultStructure(TemplateType type);
    static std::vector<TemplateFile> getDefaultFiles(TemplateType type);
    static std::vector<std::string> getDefaultFeatures(TemplateType type);
    static DirectoryStructure getMinimalStructure(TemplateType type);
    static DirectoryStructure getAdvancedStructure(TemplateType type);
    static DirectoryStructure parseCustomStructure(const std::string& directoriesStr, const std::string& filesStr);

    // Template registry
    static std::vector<TemplateCustomization> s_registeredTemplates;
    static bool s_initialized;

    // Initialization
    static void initializeDefaultTemplates();
    static void loadCustomTemplates();

    // File system operations
    static bool createDirectoryStructure(
        const std::filesystem::path& basePath,
        const DirectoryStructure& structure
    );
    static bool writeTemplateFiles(
        const std::filesystem::path& basePath,
        const std::vector<TemplateFile>& files,
        const std::unordered_map<std::string, std::string>& placeholders
    );

    // Interactive helpers
    static void showTemplatePreview(const TemplateCustomization& customization);
    static void showFeatureDetails(const std::string& feature);
    static bool confirmCustomization(const TemplateCustomization& customization);
};

// Template builder for creating custom templates programmatically
class TemplateBuilder {
public:
    TemplateBuilder(const std::string& name);

    // Fluent interface for building templates
    TemplateBuilder& withDescription(const std::string& description);
    TemplateBuilder& withDirectory(const std::string& directory);
    TemplateBuilder& withFile(const TemplateFile& file);
    TemplateBuilder& withFeature(const std::string& feature, bool defaultEnabled = false);
    TemplateBuilder& withPlaceholder(const std::string& key, const std::string& defaultValue);
    TemplateBuilder& withBuildSystem(const std::string& buildSystem);
    TemplateBuilder& withPackage(const std::string& package);

    // Build the final template customization
    TemplateCustomization build();

private:
    TemplateCustomization m_customization;
};

// Predefined template configurations
namespace predefined {
    TemplateCustomization getConsoleTemplate();
    TemplateCustomization getLibraryTemplate();
    TemplateCustomization getHeaderOnlyLibTemplate();
    TemplateCustomization getGuiTemplate();
    TemplateCustomization getNetworkTemplate();
    TemplateCustomization getWebServiceTemplate();
    TemplateCustomization getEmbeddedTemplate();
    TemplateCustomization getGameEngineTemplate();
}

} // namespace templates
