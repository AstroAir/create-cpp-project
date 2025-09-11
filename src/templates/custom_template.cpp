#include "custom_template.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <fstream>

#include "../utils/file_utils.h"
#include "../utils/string_utils.h"

#include "console_template.h"
#include "gui_template.h"
#include "library_template.h"
#include "network_template.h"

CustomTemplate::CustomTemplate(const CliOptions& options) : TemplateBase(options) {
    m_metadata.name = "custom";
    m_metadata.description = "Custom user-defined template";
    m_metadata.version = "1.0.0";
    m_metadata.author = "User";
}

bool CustomTemplate::create() {
    return createTemplate();
}

bool CustomTemplate::createProjectStructure() {
    // Create custom directories first
    if (!createCustomDirectories(options_.projectName)) {
        return false;
    }

    // Create custom files
    if (!createCustomFiles(options_.projectName)) {
        return false;
    }

    return true;
}

bool CustomTemplate::createBuildSystem() {
    // Otherwise, create a basic CMakeLists.txt if none exists
    std::string cmakeFile = utils::FileUtils::combinePath(options_.projectName, "CMakeLists.txt");
    if (!utils::FileUtils::fileExists(cmakeFile)) {
        std::string content = "cmake_minimum_required(VERSION 3.10)\n";
        content += "project(" + options_.projectName + ")\n\n";
        content += "set(CMAKE_CXX_STANDARD 17)\n";
        content += "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
        content += "add_executable(" + options_.projectName + " main.cpp)\n";

        return utils::FileUtils::writeToFile(cmakeFile, content);
    }

    return true;
}

bool CustomTemplate::setupPackageManager() {
    return true;  // No package manager setup needed for basic custom template
}

bool CustomTemplate::setupTestFramework() {
    return true;  // No test framework setup needed for basic custom template
}

bool CustomTemplate::createTemplate() {
    try {
        spdlog::info("Creating custom template: {}", m_metadata.name);

        // Create base template if specified
        if (!m_metadata.baseTemplate.empty()) {
            auto baseTemplate = createBaseTemplate();
            if (baseTemplate) {
                if (!baseTemplate->create()) {
                    spdlog::error("Failed to create base template");
                    return false;
                }

                if (!mergeWithBaseTemplate(*baseTemplate)) {
                    spdlog::error("Failed to merge with base template");
                    return false;
                }
            }
        }

        // Create custom directories
        if (!createCustomDirectories(options_.projectName)) {
            spdlog::error("Failed to create custom directories");
            return false;
        }

        // Create custom files
        if (!createCustomFiles(options_.projectName)) {
            spdlog::error("Failed to create custom files");
            return false;
        }

        spdlog::info("Custom template created successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error creating custom template: {}", e.what());
        return false;
    }
}

std::string CustomTemplate::getDescription() const {
    return m_metadata.description;
}

bool CustomTemplate::loadFromFile(const std::filesystem::path& templatePath) {
    try {
        if (!std::filesystem::exists(templatePath)) {
            spdlog::error("Template file does not exist: {}", templatePath.string());
            return false;
        }

        std::ifstream file(templatePath);
        if (!file.is_open()) {
            spdlog::error("Failed to open template file: {}", templatePath.string());
            return false;
        }

        json templateJson;
        file >> templateJson;

        return fromJson(templateJson);

    } catch (const std::exception& e) {
        spdlog::error("Error loading template from file: {}", e.what());
        return false;
    }
}

bool CustomTemplate::saveToFile(const std::filesystem::path& templatePath) const {
    try {
        // Ensure directory exists
        std::filesystem::create_directories(templatePath.parent_path());

        std::ofstream file(templatePath);
        if (!file.is_open()) {
            spdlog::error("Failed to create template file: {}", templatePath.string());
            return false;
        }

        json templateJson = toJson();
        file << templateJson.dump(2);

        spdlog::info("Template saved to: {}", templatePath.string());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error saving template to file: {}", e.what());
        return false;
    }
}

void CustomTemplate::setCustomProperty(const std::string& key, const json& value) {
    m_metadata.customProperties[key] = value;
}

std::optional<json> CustomTemplate::getCustomProperty(const std::string& key) const {
    auto it = m_metadata.customProperties.find(key);
    if (it != m_metadata.customProperties.end()) {
        return it.value();
    }
    return std::nullopt;
}

bool CustomTemplate::addCustomFile(const std::string& relativePath, const std::string& content) {
    FileEntry entry;
    entry.relativePath = relativePath;
    entry.content = content;
    entry.isTemplate = content.find("{{") != std::string::npos;  // Simple template detection

    m_customFiles.push_back(entry);
    return true;
}

bool CustomTemplate::addCustomDirectory(const std::string& relativePath) {
    DirectoryEntry entry;
    entry.relativePath = relativePath;

    m_customDirectories.push_back(entry);
    return true;
}

void CustomTemplate::setTemplateVariable(const std::string& name, const std::string& value) {
    m_metadata.variables[name] = value;
}

std::string CustomTemplate::getTemplateVariable(const std::string& name) const {
    auto it = m_metadata.variables.find(name);
    if (it != m_metadata.variables.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> CustomTemplate::getAllTemplateVariables() const {
    return m_metadata.variables;
}

bool CustomTemplate::validateTemplate() const {
    // Basic validation
    if (m_metadata.name.empty()) {
        return false;
    }

    if (m_metadata.description.empty()) {
        return false;
    }

    // Validate file paths
    for (const auto& file : m_customFiles) {
        if (file.relativePath.empty()) {
            return false;
        }

        // Check for invalid characters
        if (file.relativePath.find("..") != std::string::npos) {
            return false;
        }
    }

    // Validate directory paths
    for (const auto& dir : m_customDirectories) {
        if (dir.relativePath.empty()) {
            return false;
        }

        if (dir.relativePath.find("..") != std::string::npos) {
            return false;
        }
    }

    return true;
}

std::string CustomTemplate::processTemplateContent(const std::string& content) const {
    return replaceTemplateVariables(content);
}

std::string CustomTemplate::replaceTemplateVariables(const std::string& content) const {
    std::string result = content;

    // Replace standard variables
    result = utils::StringUtils::replace(result, "{{PROJECT_NAME}}", options_.projectName);
    result = utils::StringUtils::replace(result, "{{PROJECT_NAME_UPPER}}",
                                         utils::StringUtils::toUpper(options_.projectName));
    result = utils::StringUtils::replace(result, "{{PROJECT_NAME_LOWER}}",
                                         utils::StringUtils::toLower(options_.projectName));

    // Replace custom variables
    for (const auto& [name, value] : m_metadata.variables) {
        std::string placeholder = "{{" + name + "}}";
        result = utils::StringUtils::replace(result, placeholder, value);
    }

    // Replace current date/time
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);

    char dateBuffer[32];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &tm);
    result = utils::StringUtils::replace(result, "{{CURRENT_DATE}}", std::string(dateBuffer));

    std::strftime(dateBuffer, sizeof(dateBuffer), "%Y", &tm);
    result = utils::StringUtils::replace(result, "{{CURRENT_YEAR}}", std::string(dateBuffer));

    return result;
}

json CustomTemplate::toJson() const {
    json templateJson;

    // Metadata
    templateJson["metadata"]["name"] = m_metadata.name;
    templateJson["metadata"]["description"] = m_metadata.description;
    templateJson["metadata"]["version"] = m_metadata.version;
    templateJson["metadata"]["author"] = m_metadata.author;
    templateJson["metadata"]["baseTemplate"] = m_metadata.baseTemplate;
    templateJson["metadata"]["tags"] = m_metadata.tags;
    templateJson["metadata"]["variables"] = m_metadata.variables;
    templateJson["metadata"]["customProperties"] = m_metadata.customProperties;

    // Files
    for (const auto& file : m_customFiles) {
        json fileJson;
        fileJson["relativePath"] = file.relativePath;
        fileJson["content"] = file.content;
        fileJson["isTemplate"] = file.isTemplate;
        fileJson["metadata"] = file.metadata;
        templateJson["files"].push_back(fileJson);
    }

    // Directories
    for (const auto& dir : m_customDirectories) {
        json dirJson;
        dirJson["relativePath"] = dir.relativePath;
        dirJson["metadata"] = dir.metadata;
        templateJson["directories"].push_back(dirJson);
    }

    return templateJson;
}

bool CustomTemplate::fromJson(const json& templateJson) {
    try {
        // Load metadata
        if (templateJson.contains("metadata")) {
            const auto& meta = templateJson["metadata"];
            m_metadata.name = meta.value("name", "");
            m_metadata.description = meta.value("description", "");
            m_metadata.version = meta.value("version", "1.0.0");
            m_metadata.author = meta.value("author", "");
            m_metadata.baseTemplate = meta.value("baseTemplate", "");

            if (meta.contains("tags")) {
                m_metadata.tags = meta["tags"].get<std::vector<std::string>>();
            }

            if (meta.contains("variables")) {
                m_metadata.variables = meta["variables"].get<std::map<std::string, std::string>>();
            }

            if (meta.contains("customProperties")) {
                m_metadata.customProperties = meta["customProperties"];
            }
        }

        // Load files
        if (templateJson.contains("files")) {
            m_customFiles.clear();
            for (const auto& fileJson : templateJson["files"]) {
                FileEntry file;
                file.relativePath = fileJson.value("relativePath", "");
                file.content = fileJson.value("content", "");
                file.isTemplate = fileJson.value("isTemplate", false);

                if (fileJson.contains("metadata")) {
                    file.metadata = fileJson["metadata"].get<std::map<std::string, std::string>>();
                }

                m_customFiles.push_back(file);
            }
        }

        // Load directories
        if (templateJson.contains("directories")) {
            m_customDirectories.clear();
            for (const auto& dirJson : templateJson["directories"]) {
                DirectoryEntry dir;
                dir.relativePath = dirJson.value("relativePath", "");

                if (dirJson.contains("metadata")) {
                    dir.metadata = dirJson["metadata"].get<std::map<std::string, std::string>>();
                }

                m_customDirectories.push_back(dir);
            }
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error parsing template JSON: {}", e.what());
        return false;
    }
}

bool CustomTemplate::createCustomFiles(const std::string& projectPath) const {
    for (const auto& file : m_customFiles) {
        std::string fullPath = utils::FileUtils::combinePath(projectPath, file.relativePath);

        // Ensure directory exists
        std::filesystem::path filePath(fullPath);
        std::filesystem::create_directories(filePath.parent_path());

        // Process content
        std::string content = file.isTemplate ? processTemplateContent(file.content) : file.content;

        if (!utils::FileUtils::writeToFile(fullPath, content)) {
            spdlog::error("Failed to create custom file: {}", fullPath);
            return false;
        }

        spdlog::debug("Created custom file: {}", fullPath);
    }

    return true;
}

bool CustomTemplate::createCustomDirectories(const std::string& projectPath) const {
    for (const auto& dir : m_customDirectories) {
        std::string fullPath = utils::FileUtils::combinePath(projectPath, dir.relativePath);

        if (!utils::FileUtils::createDirectory(fullPath)) {
            spdlog::error("Failed to create custom directory: {}", fullPath);
            return false;
        }

        spdlog::debug("Created custom directory: {}", fullPath);
    }

    return true;
}

std::unique_ptr<TemplateBase> CustomTemplate::createBaseTemplate() const {
    if (m_metadata.baseTemplate == "console") {
        return std::make_unique<ConsoleTemplate>(options_);
    } else if (m_metadata.baseTemplate == "library") {
        return std::make_unique<LibraryTemplate>(options_);
    } else if (m_metadata.baseTemplate == "gui") {
        return std::make_unique<GuiTemplate>(options_);
    } else if (m_metadata.baseTemplate == "network") {
        return std::make_unique<NetworkTemplate>(options_);
    }

    return nullptr;
}

bool CustomTemplate::mergeWithBaseTemplate(const TemplateBase& baseTemplate) {
    // This is a simplified merge - in a full implementation,
    // you would merge files, directories, and other template properties
    (void)baseTemplate;  // TODO: Implement actual merging logic
    spdlog::info("Merging with base template: {}", m_metadata.baseTemplate);
    return true;
}

// CustomTemplateManager Implementation
CustomTemplateManager& CustomTemplateManager::getInstance() {
    static CustomTemplateManager instance;
    return instance;
}

std::vector<std::string> CustomTemplateManager::listAvailableTemplates() const {
    std::vector<std::string> templates;

    try {
        auto templateFiles = scanTemplateFiles();

        for (const auto& filePath : templateFiles) {
            std::string templateName = filePath.stem().string();
            templates.push_back(templateName);
        }

    } catch (const std::exception& e) {
        spdlog::error("Error listing templates: {}", e.what());
    }

    return templates;
}

std::unique_ptr<CustomTemplate> CustomTemplateManager::loadTemplate(
        const std::string& templateName) const {
    try {
        auto templatePath = getTemplateFilePath(templateName);

        if (!std::filesystem::exists(templatePath)) {
            spdlog::error("Template not found: {}", templateName);
            return nullptr;
        }

        CliOptions dummyOptions;  // Will be overridden when used
        auto customTemplate = std::make_unique<CustomTemplate>(dummyOptions);

        if (!customTemplate->loadFromFile(templatePath)) {
            spdlog::error("Failed to load template: {}", templateName);
            return nullptr;
        }

        return customTemplate;

    } catch (const std::exception& e) {
        spdlog::error("Error loading template {}: {}", templateName, e.what());
        return nullptr;
    }
}

bool CustomTemplateManager::saveTemplate(const CustomTemplate& customTemplate,
                                         const std::string& templateName) {
    try {
        if (!ensureTemplateDirectoriesExist()) {
            return false;
        }

        auto templatePath = getTemplateFilePath(templateName);
        return customTemplate.saveToFile(templatePath);

    } catch (const std::exception& e) {
        spdlog::error("Error saving template {}: {}", templateName, e.what());
        return false;
    }
}

bool CustomTemplateManager::deleteTemplate(const std::string& templateName) {
    try {
        auto templatePath = getTemplateFilePath(templateName);

        if (!std::filesystem::exists(templatePath)) {
            spdlog::error("Template not found: {}", templateName);
            return false;
        }

        std::filesystem::remove(templatePath);
        spdlog::info("Template deleted: {}", templateName);
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error deleting template {}: {}", templateName, e.what());
        return false;
    }
}

std::filesystem::path CustomTemplateManager::getTemplatesDirectory() const {
    return getUserTemplatesDirectory();
}

std::filesystem::path CustomTemplateManager::getUserTemplatesDirectory() const {
#ifdef _WIN32
    std::string appData = std::getenv("APPDATA") ? std::getenv("APPDATA") : "";
    if (!appData.empty()) {
        return std::filesystem::path(appData) / "cpp-scaffold" / "templates";
    }
    return std::filesystem::path("templates");
#else
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
    if (!home.empty()) {
        return std::filesystem::path(home) / ".config" / "cpp-scaffold" / "templates";
    }
    return std::filesystem::path("templates");
#endif
}

std::filesystem::path CustomTemplateManager::getSystemTemplatesDirectory() const {
    // System-wide templates directory
#ifdef _WIN32
    return std::filesystem::path("C:/ProgramData/cpp-scaffold/templates");
#else
    return std::filesystem::path("/usr/share/cpp-scaffold/templates");
#endif
}

std::filesystem::path CustomTemplateManager::getTemplateFilePath(
        const std::string& templateName) const {
    return getUserTemplatesDirectory() / (templateName + ".json");
}

bool CustomTemplateManager::ensureTemplateDirectoriesExist() const {
    try {
        auto userDir = getUserTemplatesDirectory();
        std::filesystem::create_directories(userDir);
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error creating template directories: {}", e.what());
        return false;
    }
}

std::vector<std::filesystem::path> CustomTemplateManager::scanTemplateFiles() const {
    std::vector<std::filesystem::path> templateFiles;

    // Scan user templates
    auto userTemplates = scanDirectory(getUserTemplatesDirectory());
    templateFiles.insert(templateFiles.end(), userTemplates.begin(), userTemplates.end());

    // Scan system templates
    auto systemTemplates = scanDirectory(getSystemTemplatesDirectory());
    templateFiles.insert(templateFiles.end(), systemTemplates.begin(), systemTemplates.end());

    return templateFiles;
}

std::vector<std::filesystem::path> CustomTemplateManager::scanDirectory(
        const std::filesystem::path& directory) const {
    std::vector<std::filesystem::path> templateFiles;

    try {
        if (!std::filesystem::exists(directory)) {
            return templateFiles;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                templateFiles.push_back(entry.path());
            }
        }

    } catch (const std::exception& e) {
        spdlog::error("Error scanning directory {}: {}", directory.string(), e.what());
    }

    return templateFiles;
}
