#pragma once

#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>

#include "template_base.h"

using json = nlohmann::json;

// Custom template system for user-defined project templates
class CustomTemplate : public TemplateBase {
    friend class CustomTemplateManager;
    friend class TemplateManager;

   public:
    explicit CustomTemplate(const CliOptions& options);

    // TemplateBase interface implementation
    bool create() override;

    // Template creation and management
    bool createTemplate();
    std::string getDescription() const;

   protected:
    // TemplateBase pure virtual methods
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

    // Custom template specific methods
    bool loadFromFile(const std::filesystem::path& templatePath);
    bool saveToFile(const std::filesystem::path& templatePath) const;

    // Template inheritance
    bool inheritFrom(const std::string& baseTemplateName);

    // Template customization
    void setCustomProperty(const std::string& key, const json& value);
    std::optional<json> getCustomProperty(const std::string& key) const;

    // File and directory operations
    bool addCustomFile(const std::string& relativePath, const std::string& content);
    bool addCustomDirectory(const std::string& relativePath);
    bool removeCustomFile(const std::string& relativePath);

    // Template variables
    void setTemplateVariable(const std::string& name, const std::string& value);
    std::string getTemplateVariable(const std::string& name) const;
    std::map<std::string, std::string> getAllTemplateVariables() const;

    // Template validation
    bool validateTemplate() const;
    std::vector<std::string> getValidationErrors() const;

   private:
    // Template metadata
    struct TemplateMetadata {
        std::string name;
        std::string description;
        std::string version;
        std::string author;
        std::string baseTemplate;
        std::vector<std::string> tags;
        std::map<std::string, std::string> variables;
        json customProperties;
    };

    // File entry for custom files
    struct FileEntry {
        std::string relativePath;
        std::string content;
        bool isTemplate;  // Whether content contains template variables
        std::map<std::string, std::string> metadata;
    };

    // Directory entry
    struct DirectoryEntry {
        std::string relativePath;
        std::map<std::string, std::string> metadata;
    };

    TemplateMetadata m_metadata;
    std::vector<FileEntry> m_customFiles;
    std::vector<DirectoryEntry> m_customDirectories;

    // Template processing
    std::string processTemplateContent(const std::string& content) const;
    std::string replaceTemplateVariables(const std::string& content) const;

    // JSON serialization
    json toJson() const;
    bool fromJson(const json& templateJson);

    // File operations
    bool createCustomFiles(const std::string& projectPath) const;
    bool createCustomDirectories(const std::string& projectPath) const;

    // Base template integration
    std::unique_ptr<TemplateBase> createBaseTemplate() const;
    bool mergeWithBaseTemplate(const TemplateBase& baseTemplate);
};

// Custom template manager for handling multiple templates
class CustomTemplateManager {
   public:
    static CustomTemplateManager& getInstance();

    // Template discovery and loading
    std::vector<std::string> listAvailableTemplates() const;
    std::unique_ptr<CustomTemplate> loadTemplate(const std::string& templateName) const;

    // Template creation and saving
    bool saveTemplate(const CustomTemplate& customTemplate, const std::string& templateName);
    bool deleteTemplate(const std::string& templateName);

    // Template directories
    std::filesystem::path getTemplatesDirectory() const;
    std::filesystem::path getUserTemplatesDirectory() const;
    std::filesystem::path getSystemTemplatesDirectory() const;

    // Template validation and import/export
    bool validateTemplateFile(const std::filesystem::path& templatePath) const;
    bool importTemplate(const std::filesystem::path& templatePath, const std::string& templateName);
    bool exportTemplate(const std::string& templateName,
                        const std::filesystem::path& exportPath) const;

    // Template search and filtering
    std::vector<std::string> searchTemplates(const std::string& query) const;
    std::vector<std::string> getTemplatesByTag(const std::string& tag) const;
    std::vector<std::string> getTemplatesByAuthor(const std::string& author) const;

    // Template metadata
    struct TemplateInfo {
        std::string name;
        std::string description;
        std::string version;
        std::string author;
        std::vector<std::string> tags;
        std::filesystem::path filePath;
        std::chrono::system_clock::time_point lastModified;
    };

    std::vector<TemplateInfo> getTemplateInfoList() const;
    std::optional<TemplateInfo> getTemplateInfo(const std::string& templateName) const;

   private:
    CustomTemplateManager() = default;

    // Template file operations
    std::filesystem::path getTemplateFilePath(const std::string& templateName) const;
    bool ensureTemplateDirectoriesExist() const;

    // Template scanning
    std::vector<std::filesystem::path> scanTemplateFiles() const;
    std::vector<std::filesystem::path> scanDirectory(const std::filesystem::path& directory) const;
};

// Template builder for creating custom templates interactively
class CustomTemplateBuilder {
   public:
    CustomTemplateBuilder();

    // Interactive template creation
    CustomTemplate buildInteractively();

    // Step-by-step template creation
    void setBasicInfo(const std::string& name, const std::string& description);
    void setAuthorInfo(const std::string& author, const std::string& version = "1.0.0");
    void addTags(const std::vector<std::string>& tags);
    void setBaseTemplate(const std::string& baseTemplateName);

    // File and directory addition
    void addFileInteractively();
    void addDirectoryInteractively();
    void addFileFromPath(const std::filesystem::path& sourcePath, const std::string& targetPath);

    // Variable management
    void addVariableInteractively();
    void addVariable(const std::string& name, const std::string& defaultValue,
                     const std::string& description = "");

    // Template finalization
    CustomTemplate build();
    bool saveTemplate(const std::string& templateName);

   private:
    CustomTemplate m_template;

    // Interactive helpers
    std::string promptForString(const std::string& prompt, const std::string& defaultValue = "");
    bool promptForBool(const std::string& prompt, bool defaultValue = false);
    std::vector<std::string> promptForTags();
    std::string promptForFileContent(const std::string& filePath);

    // Validation helpers
    bool isValidTemplateName(const std::string& name) const;
    bool isValidVariableName(const std::string& name) const;
};

// Template inheritance system
class TemplateInheritance {
   public:
    // Inheritance operations
    static bool canInheritFrom(const std::string& baseTemplateName);
    static std::unique_ptr<TemplateBase> createBaseTemplate(const std::string& baseTemplateName,
                                                            const CliOptions& options);

    // Inheritance chain
    static std::vector<std::string> getInheritanceChain(const std::string& templateName);
    static bool hasCircularInheritance(const std::string& templateName);

    // Template merging
    static bool mergeTemplates(CustomTemplate& derived, const TemplateBase& base);
    static bool mergeFiles(CustomTemplate& derived, const TemplateBase& base);
    static bool mergeDirectories(CustomTemplate& derived, const TemplateBase& base);

   private:
    // Built-in template mapping
    static std::map<std::string, std::function<std::unique_ptr<TemplateBase>(const CliOptions&)>>
            s_builtinTemplates;

    // Inheritance validation
    static bool validateInheritance(const std::string& derived, const std::string& base);
};

// Template variable system
class TemplateVariables {
   public:
    // Variable types
    enum class VariableType { String, Integer, Boolean, Choice, Path, Email, URL };

    // Variable definition
    struct VariableDefinition {
        std::string name;
        VariableType type;
        std::string description;
        std::string defaultValue;
        std::vector<std::string> choices;  // For Choice type
        bool required;
        std::string validationRegex;
    };

    // Variable management
    void addVariable(const VariableDefinition& variable);
    void removeVariable(const std::string& name);
    std::optional<VariableDefinition> getVariable(const std::string& name) const;
    std::vector<VariableDefinition> getAllVariables() const;

    // Variable values
    void setValue(const std::string& name, const std::string& value);
    std::string getValue(const std::string& name) const;
    std::map<std::string, std::string> getAllValues() const;

    // Variable validation
    bool validateValue(const std::string& name, const std::string& value) const;
    std::vector<std::string> getValidationErrors() const;

    // Interactive variable collection
    bool collectValuesInteractively();

   private:
    std::map<std::string, VariableDefinition> m_variables;
    std::map<std::string, std::string> m_values;

    // Validation helpers
    bool validateString(const std::string& value, const VariableDefinition& var) const;
    bool validateInteger(const std::string& value) const;
    bool validateBoolean(const std::string& value) const;
    bool validateChoice(const std::string& value, const VariableDefinition& var) const;
    bool validatePath(const std::string& value) const;
    bool validateEmail(const std::string& value) const;
    bool validateURL(const std::string& value) const;
    bool validateRegex(const std::string& value, const std::string& regex) const;
};
