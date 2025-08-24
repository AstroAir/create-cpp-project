#include "enhanced_config_manager.h"
#include "../utils/terminal_utils.h"
#include "../utils/file_utils.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <algorithm>
#include <regex>

namespace config {

EnhancedConfigManager& EnhancedConfigManager::getInstance() {
    static EnhancedConfigManager instance;
    static bool initialized = false;
    
    if (!initialized) {
        instance.initializeDefaultConfiguration();
        instance.initializeBuiltInTemplates();
        initialized = true;
    }
    
    return instance;
}

bool EnhancedConfigManager::loadConfiguration(ConfigScope scope) {
    try {
        std::filesystem::path configPath = getConfigFilePath(scope);
        
        if (!std::filesystem::exists(configPath)) {
            if (scope == ConfigScope::User) {
                // Create default user configuration
                initializeDefaultConfiguration();
                return saveConfiguration(scope);
            }
            return true; // No configuration file is okay for other scopes
        }
        
        return loadConfigurationFile(configPath, scope);
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to load configuration for scope {}: {}", static_cast<int>(scope), e.what());
        return false;
    }
}

bool EnhancedConfigManager::saveConfiguration(ConfigScope scope) {
    try {
        if (!ensureConfigDirectoryExists(scope)) {
            return false;
        }
        
        std::filesystem::path configPath = getConfigFilePath(scope);
        return saveConfigurationFile(configPath, scope);
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to save configuration for scope {}: {}", static_cast<int>(scope), e.what());
        return false;
    }
}

template<typename T>
std::optional<T> EnhancedConfigManager::getValue(const std::string& key, ConfigScope scope) {
    try {
        nlohmann::json effectiveValue = getEffectiveValue(key);
        
        if (effectiveValue.is_null()) {
            return std::nullopt;
        }
        
        return config_utils::convertValue<T>(effectiveValue);
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to get value for key '{}': {}", key, e.what());
        return std::nullopt;
    }
}

template<typename T>
bool EnhancedConfigManager::setValue(const std::string& key, const T& value, ConfigScope scope) {
    try {
        nlohmann::json jsonValue = value;
        
        // Validate the value
        if (!validateKey(key, jsonValue)) {
            return false;
        }
        
        // Get old value for change notification
        nlohmann::json oldValue = getEffectiveValue(key);
        
        // Set the value in the appropriate scope
        if (configurations_.find(scope) == configurations_.end()) {
            configurations_[scope] = nlohmann::json::object();
        }
        
        // Parse key path and set nested value
        auto keyPath = config_utils::parseKeyPath(key);
        nlohmann::json* current = &configurations_[scope];
        
        for (size_t i = 0; i < keyPath.size() - 1; ++i) {
            if (!current->contains(keyPath[i])) {
                (*current)[keyPath[i]] = nlohmann::json::object();
            }
            current = &(*current)[keyPath[i]];
        }
        
        (*current)[keyPath.back()] = jsonValue;
        
        // Notify change listeners
        notifyConfigChange(key, oldValue, jsonValue, ConfigSource::UserConfig);
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to set value for key '{}': {}", key, e.what());
        return false;
    }
}

nlohmann::json EnhancedConfigManager::getEffectiveValue(const std::string& key) {
    // Check configuration hierarchy: CommandLine > Environment > User > System > Default
    
    // Check environment overrides first
    auto envIt = environmentOverrides_.find(key);
    if (envIt != environmentOverrides_.end()) {
        return envIt->second;
    }
    
    // Check user configuration
    if (configurations_.find(ConfigScope::User) != configurations_.end()) {
        auto keyPath = config_utils::parseKeyPath(key);
        nlohmann::json* current = &configurations_[ConfigScope::User];
        
        for (const auto& pathComponent : keyPath) {
            if (current->contains(pathComponent)) {
                current = &(*current)[pathComponent];
            } else {
                current = nullptr;
                break;
            }
        }
        
        if (current && !current->is_null()) {
            return *current;
        }
    }
    
    // Check system configuration
    if (configurations_.find(ConfigScope::Global) != configurations_.end()) {
        auto keyPath = config_utils::parseKeyPath(key);
        nlohmann::json* current = &configurations_[ConfigScope::Global];
        
        for (const auto& pathComponent : keyPath) {
            if (current->contains(pathComponent)) {
                current = &(*current)[pathComponent];
            } else {
                current = nullptr;
                break;
            }
        }
        
        if (current && !current->is_null()) {
            return *current;
        }
    }
    
    // Return default value if available
    auto entryIt = configEntries_.find(key);
    if (entryIt != configEntries_.end()) {
        return entryIt->second.value;
    }
    
    return nlohmann::json{};
}

CliOptions EnhancedConfigManager::getDefaultOptions() {
    CliOptions options;
    
    // Load default values from configuration
    options.templateType = getValue<std::string>("defaults.template", ConfigScope::User)
                          .and_then([](const std::string& s) { return enums::to_template_type(s); })
                          .value_or(TemplateType::Console);
    
    options.buildSystem = getValue<std::string>("defaults.build_system", ConfigScope::User)
                         .and_then([](const std::string& s) { return enums::to_build_system(s); })
                         .value_or(BuildSystem::CMake);
    
    options.packageManager = getValue<std::string>("defaults.package_manager", ConfigScope::User)
                            .and_then([](const std::string& s) { return enums::to_package_manager(s); })
                            .value_or(PackageManager::Vcpkg);
    
    options.testFramework = getValue<std::string>("defaults.test_framework", ConfigScope::User)
                           .and_then([](const std::string& s) { return enums::to_test_framework(s); })
                           .value_or(TestFramework::GTest);
    
    options.includeTests = getValue<bool>("defaults.include_tests", ConfigScope::User).value_or(false);
    options.includeDocumentation = getValue<bool>("defaults.include_documentation", ConfigScope::User).value_or(false);
    options.includeCodeStyleTools = getValue<bool>("defaults.include_code_style", ConfigScope::User).value_or(false);
    options.initGit = getValue<bool>("defaults.init_git", ConfigScope::User).value_or(true);
    options.verbose = getValue<bool>("defaults.verbose", ConfigScope::User).value_or(false);
    
    return options;
}

bool EnhancedConfigManager::setDefaultOptions(const CliOptions& options) {
    try {
        setValue("defaults.template", std::string(enums::to_string(options.templateType)));
        setValue("defaults.build_system", std::string(enums::to_string(options.buildSystem)));
        setValue("defaults.package_manager", std::string(enums::to_string(options.packageManager)));
        setValue("defaults.test_framework", std::string(enums::to_string(options.testFramework)));
        setValue("defaults.include_tests", options.includeTests);
        setValue("defaults.include_documentation", options.includeDocumentation);
        setValue("defaults.include_code_style", options.includeCodeStyleTools);
        setValue("defaults.init_git", options.initGit);
        setValue("defaults.verbose", options.verbose);
        
        return saveConfiguration(ConfigScope::User);
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to set default options: {}", e.what());
        return false;
    }
}

bool EnhancedConfigManager::configureInteractively() {
    utils::TerminalUtils::showNpmStyleHeader("Configuration Setup");
    
    std::cout << "\n" << utils::TerminalUtils::colorize("  Let's configure your default settings", utils::Color::BrightWhite) << "\n\n";
    
    // Get current defaults
    CliOptions currentDefaults = getDefaultOptions();
    
    // Configure default template
    std::vector<std::string> templates = {
        "console - Console applications",
        "lib - Library projects", 
        "header-only-lib - Header-only libraries",
        "gui - GUI applications",
        "network - Network applications",
        "embedded - Embedded projects",
        "webservice - Web services",
        "gameengine - Game engines"
    };
    
    int templateChoice = utils::TerminalUtils::showInteractiveMenu(
        templates, "Default project template", static_cast<int>(currentDefaults.templateType));
    
    if (templateChoice >= 0 && templateChoice < static_cast<int>(templates.size())) {
        currentDefaults.templateType = static_cast<TemplateType>(templateChoice);
    }
    
    // Configure default build system
    std::vector<std::string> buildSystems = {
        "cmake - CMake build system",
        "meson - Meson build system",
        "bazel - Bazel build system",
        "xmake - XMake build system",
        "premake - Premake build system",
        "make - GNU Make",
        "ninja - Ninja build system"
    };
    
    int buildChoice = utils::TerminalUtils::showInteractiveMenu(
        buildSystems, "Default build system", static_cast<int>(currentDefaults.buildSystem));
    
    if (buildChoice >= 0 && buildChoice < static_cast<int>(buildSystems.size())) {
        currentDefaults.buildSystem = static_cast<BuildSystem>(buildChoice);
    }
    
    // Configure default package manager
    std::vector<std::string> packageManagers = {
        "vcpkg - Microsoft vcpkg",
        "conan - Conan package manager",
        "none - No package manager",
        "spack - Spack package manager",
        "hunter - Hunter package manager",
        "cpm - CPM.cmake",
        "fetchcontent - CMake FetchContent"
    };
    
    int packageChoice = utils::TerminalUtils::showInteractiveMenu(
        packageManagers, "Default package manager", static_cast<int>(currentDefaults.packageManager));
    
    if (packageChoice >= 0 && packageChoice < static_cast<int>(packageManagers.size())) {
        currentDefaults.packageManager = static_cast<PackageManager>(packageChoice);
    }
    
    // Configure boolean options
    std::vector<std::string> booleanOptions = {
        "Include tests by default",
        "Include documentation by default",
        "Include code style tools by default",
        "Initialize Git repository by default",
        "Enable verbose output by default"
    };
    
    std::vector<bool> currentBooleanValues = {
        currentDefaults.includeTests,
        currentDefaults.includeDocumentation,
        currentDefaults.includeCodeStyleTools,
        currentDefaults.initGit,
        currentDefaults.verbose
    };
    
    auto selectedBooleans = utils::TerminalUtils::showMultiSelectDialog(
        "Default options", booleanOptions, currentBooleanValues);
    
    // Apply boolean selections
    currentDefaults.includeTests = std::find(selectedBooleans.begin(), selectedBooleans.end(), 
                                            "Include tests by default") != selectedBooleans.end();
    currentDefaults.includeDocumentation = std::find(selectedBooleans.begin(), selectedBooleans.end(), 
                                                     "Include documentation by default") != selectedBooleans.end();
    currentDefaults.includeCodeStyleTools = std::find(selectedBooleans.begin(), selectedBooleans.end(), 
                                                      "Include code style tools by default") != selectedBooleans.end();
    currentDefaults.initGit = std::find(selectedBooleans.begin(), selectedBooleans.end(), 
                                       "Initialize Git repository by default") != selectedBooleans.end();
    currentDefaults.verbose = std::find(selectedBooleans.begin(), selectedBooleans.end(), 
                                       "Enable verbose output by default") != selectedBooleans.end();
    
    // Save the configuration
    if (setDefaultOptions(currentDefaults)) {
        utils::TerminalUtils::showNpmStyleSuccess("Configuration saved successfully");
        return true;
    } else {
        utils::TerminalUtils::showNpmStyleError("Failed to save configuration");
        return false;
    }
}

void EnhancedConfigManager::initializeDefaultConfiguration() {
    // Initialize default configuration entries
    ConfigEntry templateEntry;
    templateEntry.key = "defaults.template";
    templateEntry.value = "console";
    templateEntry.source = ConfigSource::Default;
    templateEntry.scope = ConfigScope::User;
    templateEntry.description = "Default project template type";
    templateEntry.allowedValues = {"console", "lib", "header-only-lib", "gui", "network", "embedded", "webservice", "gameengine"};
    configEntries_[templateEntry.key] = templateEntry;
    
    ConfigEntry buildSystemEntry;
    buildSystemEntry.key = "defaults.build_system";
    buildSystemEntry.value = "cmake";
    buildSystemEntry.source = ConfigSource::Default;
    buildSystemEntry.scope = ConfigScope::User;
    buildSystemEntry.description = "Default build system";
    buildSystemEntry.allowedValues = {"cmake", "meson", "bazel", "xmake", "premake", "make", "ninja"};
    configEntries_[buildSystemEntry.key] = buildSystemEntry;
    
    ConfigEntry packageManagerEntry;
    packageManagerEntry.key = "defaults.package_manager";
    packageManagerEntry.value = "vcpkg";
    packageManagerEntry.source = ConfigSource::Default;
    packageManagerEntry.scope = ConfigScope::User;
    packageManagerEntry.description = "Default package manager";
    packageManagerEntry.allowedValues = {"vcpkg", "conan", "none", "spack", "hunter", "cpm", "fetchcontent"};
    configEntries_[packageManagerEntry.key] = packageManagerEntry;
    
    ConfigEntry includeTestsEntry;
    includeTestsEntry.key = "defaults.include_tests";
    includeTestsEntry.value = false;
    includeTestsEntry.source = ConfigSource::Default;
    includeTestsEntry.scope = ConfigScope::User;
    includeTestsEntry.description = "Include tests by default";
    configEntries_[includeTestsEntry.key] = includeTestsEntry;
    
    ConfigEntry includeDocsEntry;
    includeDocsEntry.key = "defaults.include_documentation";
    includeDocsEntry.value = false;
    includeDocsEntry.source = ConfigSource::Default;
    includeDocsEntry.scope = ConfigScope::User;
    includeDocsEntry.description = "Include documentation by default";
    configEntries_[includeDocsEntry.key] = includeDocsEntry;
    
    ConfigEntry initGitEntry;
    initGitEntry.key = "defaults.init_git";
    initGitEntry.value = true;
    initGitEntry.source = ConfigSource::Default;
    initGitEntry.scope = ConfigScope::User;
    initGitEntry.description = "Initialize Git repository by default";
    configEntries_[initGitEntry.key] = initGitEntry;
}

void EnhancedConfigManager::initializeBuiltInTemplates() {
    // Web development template
    ConfigTemplate webTemplate;
    webTemplate.name = "web-development";
    webTemplate.description = "Configuration for web service development";
    webTemplate.category = "Development";
    webTemplate.isBuiltIn = true;
    webTemplate.settings["defaults.template"] = "webservice";
    webTemplate.settings["defaults.build_system"] = "cmake";
    webTemplate.settings["defaults.package_manager"] = "conan";
    webTemplate.settings["defaults.include_tests"] = true;
    webTemplate.settings["defaults.include_documentation"] = true;
    webTemplate.tags = {"web", "service", "api"};
    templates_[webTemplate.name] = webTemplate;
    
    // Game development template
    ConfigTemplate gameTemplate;
    gameTemplate.name = "game-development";
    gameTemplate.description = "Configuration for game development";
    gameTemplate.category = "Development";
    gameTemplate.isBuiltIn = true;
    gameTemplate.settings["defaults.template"] = "gameengine";
    gameTemplate.settings["defaults.build_system"] = "cmake";
    gameTemplate.settings["defaults.package_manager"] = "vcpkg";
    gameTemplate.settings["defaults.include_tests"] = true;
    gameTemplate.tags = {"game", "graphics", "engine"};
    templates_[gameTemplate.name] = gameTemplate;
    
    // Library development template
    ConfigTemplate libTemplate;
    libTemplate.name = "library-development";
    libTemplate.description = "Configuration for library development";
    libTemplate.category = "Development";
    libTemplate.isBuiltIn = true;
    libTemplate.settings["defaults.template"] = "lib";
    libTemplate.settings["defaults.build_system"] = "cmake";
    libTemplate.settings["defaults.package_manager"] = "vcpkg";
    libTemplate.settings["defaults.include_tests"] = true;
    libTemplate.settings["defaults.include_documentation"] = true;
    libTemplate.tags = {"library", "reusable", "component"};
    templates_[libTemplate.name] = libTemplate;
}

} // namespace config
