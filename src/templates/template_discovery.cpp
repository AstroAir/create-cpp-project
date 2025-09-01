#include "template_discovery.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <iostream>
#include <set>

#include "../utils/file_utils.h"
#include "../utils/terminal_utils.h"

namespace templates {

TemplateDiscovery& TemplateDiscovery::getInstance() {
    static TemplateDiscovery instance;
    static bool initialized = false;

    if (!initialized) {
        instance.initializeBuiltinTemplates();
        instance.loadCustomTemplates();
        instance.loadTemplateUsageStats();
        initialized = true;
    }

    return instance;
}

std::vector<TemplateMetadata> TemplateDiscovery::discoverTemplates() {
    std::vector<TemplateMetadata> templates;

    for (const auto& [name, metadata] : templateRegistry_) {
        templates.push_back(metadata);
    }

    return templates;
}

std::vector<TemplateMetadata> TemplateDiscovery::getAvailableTemplates() {
    std::vector<TemplateMetadata> templates;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (!metadata.isDeprecated) {
            templates.push_back(metadata);
        }
    }

    return templates;
}

std::vector<TemplateMetadata> TemplateDiscovery::getTemplatesByCategory(
        const std::string& category) {
    std::vector<TemplateMetadata> templates;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (metadata.category == category && !metadata.isDeprecated) {
            templates.push_back(metadata);
        }
    }

    return templates;
}

std::vector<TemplateMetadata> TemplateDiscovery::getTemplatesByTag(const std::string& tag) {
    std::vector<TemplateMetadata> templates;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (std::find(metadata.tags.begin(), metadata.tags.end(), tag) != metadata.tags.end() &&
            !metadata.isDeprecated) {
            templates.push_back(metadata);
        }
    }

    return templates;
}

std::vector<TemplateMetadata> TemplateDiscovery::searchTemplates(const std::string& query) {
    std::vector<TemplateMetadata> templates;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (template_utils::matchesQuery(metadata, query) && !metadata.isDeprecated) {
            templates.push_back(metadata);
        }
    }

    return template_utils::sortTemplatesByRelevance(templates, query);
}

std::optional<TemplateMetadata> TemplateDiscovery::getTemplateMetadata(
        const std::string& templateName) {
    auto it = templateRegistry_.find(templateName);
    if (it != templateRegistry_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<TemplateMetadata> TemplateDiscovery::getTemplateMetadata(TemplateType templateType) {
    std::string templateName = std::string(enums::to_string(templateType));
    return getTemplateMetadata(templateName);
}

TemplateValidationResult TemplateDiscovery::validateTemplate(const std::string& templateName) {
    TemplateValidationResult result;
    result.isValid = true;

    auto metadata = getTemplateMetadata(templateName);
    if (!metadata) {
        result.isValid = false;
        result.errors.push_back("Template not found: " + templateName);
        return result;
    }

    // Validate template metadata
    if (metadata->name.empty()) {
        result.isValid = false;
        result.errors.push_back("Template name is empty");
    }

    if (metadata->description.empty()) {
        result.warnings.push_back("Template description is empty");
    }

    if (metadata->supportedBuildSystems.empty()) {
        result.warnings.push_back("No supported build systems specified");
    }

    // Check if template is deprecated
    if (metadata->isDeprecated) {
        result.warnings.push_back("Template is deprecated");
    }

    // Check if template is experimental
    if (metadata->isExperimental) {
        result.warnings.push_back("Template is experimental and may be unstable");
    }

    return result;
}

TemplateValidationResult TemplateDiscovery::validateTemplate(TemplateType templateType) {
    std::string templateName = std::string(enums::to_string(templateType));
    return validateTemplate(templateName);
}

TemplateValidationResult TemplateDiscovery::validateTemplateWithOptions(
        const std::string& templateName, const CliOptions& options) {
    TemplateValidationResult result = validateTemplate(templateName);

    auto metadata = getTemplateMetadata(templateName);
    if (!metadata) {
        return result;
    }

    // Validate build system compatibility
    if (!validateBuildSystemCompatibility(*metadata, options.buildSystem)) {
        result.warnings.push_back("Build system may not be fully supported by this template");
    }

    // Validate package manager compatibility
    if (!validatePackageManagerCompatibility(*metadata, options.packageManager)) {
        result.warnings.push_back("Package manager may not be fully supported by this template");
    }

    return result;
}

TemplateCompatibility TemplateDiscovery::checkCompatibility(const std::string& templateName,
                                                            const CliOptions& options) {
    TemplateCompatibility compatibility;
    compatibility.isCompatible = true;
    compatibility.compatibilityScore = 100;

    auto metadata = getTemplateMetadata(templateName);
    if (!metadata) {
        compatibility.isCompatible = false;
        compatibility.compatibilityScore = 0;
        compatibility.issues.push_back("Template not found");
        return compatibility;
    }

    // Check build system compatibility
    if (!validateBuildSystemCompatibility(*metadata, options.buildSystem)) {
        compatibility.compatibilityScore -= 20;
        compatibility.warnings.push_back("Build system compatibility issues may occur");
    }

    // Check package manager compatibility
    if (!validatePackageManagerCompatibility(*metadata, options.packageManager)) {
        compatibility.compatibilityScore -= 15;
        compatibility.warnings.push_back("Package manager may require manual configuration");
    }

    // Check system compatibility
    if (!checkPlatformSupport(*metadata)) {
        compatibility.compatibilityScore -= 30;
        compatibility.issues.push_back("Platform may not be fully supported");
    }

    // Check if experimental
    if (metadata->isExperimental) {
        compatibility.compatibilityScore -= 10;
        compatibility.warnings.push_back("Template is experimental");
    }

    // Check if deprecated
    if (metadata->isDeprecated) {
        compatibility.compatibilityScore -= 25;
        compatibility.warnings.push_back("Template is deprecated");
        compatibility.suggestions.push_back("Consider using a newer template");
    }

    if (compatibility.compatibilityScore < 50) {
        compatibility.isCompatible = false;
    }

    return compatibility;
}

std::vector<std::string> TemplateDiscovery::getRecommendedTemplates(const CliOptions& options) {
    std::vector<std::pair<std::string, int>> templateScores;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (!metadata.isDeprecated) {
            int score = calculateCompatibilityScore(metadata, options);
            templateScores.emplace_back(name, score);
        }
    }

    // Sort by score (highest first)
    std::sort(templateScores.begin(), templateScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> recommendations;
    for (const auto& [name, score] : templateScores) {
        if (score >= 70) {  // Only recommend templates with good compatibility
            recommendations.push_back(name);
        }
    }

    return recommendations;
}

std::vector<std::string> TemplateDiscovery::getAvailableCategories() {
    std::set<std::string> categories;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (!metadata.isDeprecated && !metadata.category.empty()) {
            categories.insert(metadata.category);
        }
    }

    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<std::string> TemplateDiscovery::getAvailableTags() {
    std::set<std::string> tags;

    for (const auto& [name, metadata] : templateRegistry_) {
        if (!metadata.isDeprecated) {
            for (const auto& tag : metadata.tags) {
                tags.insert(tag);
            }
        }
    }

    return std::vector<std::string>(tags.begin(), tags.end());
}

void TemplateDiscovery::initializeBuiltinTemplates() {
    // Console template
    TemplateMetadata console;
    console.name = "console";
    console.displayName = "Console Application";
    console.description = "Command-line application with basic I/O functionality";
    console.category = "Application";
    console.tags = {"cli", "console", "basic", "beginner"};
    console.supportedBuildSystems = {"cmake", "meson", "bazel", "make"};
    console.supportedPackageManagers = {"vcpkg", "conan", "none"};
    console.minCppStandard = "C++11";
    console.isExperimental = false;
    console.isDeprecated = false;
    console.version = "1.0.0";
    console.author = "CPP-Scaffold Team";
    templateRegistry_["console"] = console;

    // Library template
    TemplateMetadata library;
    library.name = "lib";
    library.displayName = "Library Project";
    library.description = "Static or shared library with proper structure and testing";
    library.category = "Library";
    library.tags = {"library", "reusable", "component"};
    library.supportedBuildSystems = {"cmake", "meson", "bazel"};
    library.supportedPackageManagers = {"vcpkg", "conan", "none"};
    library.minCppStandard = "C++11";
    library.isExperimental = false;
    library.isDeprecated = false;
    library.version = "1.0.0";
    library.author = "CPP-Scaffold Team";
    templateRegistry_["lib"] = library;

    // Game Engine template
    TemplateMetadata gameEngine;
    gameEngine.name = "gameengine";
    gameEngine.displayName = "Game Engine";
    gameEngine.description = "Complete game engine with graphics, audio, and physics systems";
    gameEngine.category = "Game Development";
    gameEngine.tags = {"game", "engine", "graphics", "audio", "physics", "advanced"};
    gameEngine.supportedBuildSystems = {"cmake", "meson"};
    gameEngine.supportedPackageManagers = {"vcpkg", "conan"};
    gameEngine.requiredDependencies = {"opengl", "glfw", "glm"};
    gameEngine.optionalDependencies = {"vulkan", "openal", "bullet", "lua"};
    gameEngine.minCppStandard = "C++17";
    gameEngine.isExperimental = false;
    gameEngine.isDeprecated = false;
    gameEngine.version = "1.0.0";
    gameEngine.author = "CPP-Scaffold Team";
    templateRegistry_["gameengine"] = gameEngine;

    // Add more templates...
}

void TemplateDiscovery::loadCustomTemplates() {
    // Load custom templates from user directory
    // Implementation would scan for custom template files
}

void TemplateDiscovery::loadTemplateUsageStats() {
    // Load usage statistics from file
    // Implementation would read from a stats file
}

bool TemplateDiscovery::validateBuildSystemCompatibility(const TemplateMetadata& metadata,
                                                         BuildSystem buildSystem) {
    std::string buildSystemStr = std::string(enums::to_string(buildSystem));
    return std::find(metadata.supportedBuildSystems.begin(), metadata.supportedBuildSystems.end(),
                     buildSystemStr) != metadata.supportedBuildSystems.end();
}

bool TemplateDiscovery::validatePackageManagerCompatibility(const TemplateMetadata& metadata,
                                                            PackageManager packageManager) {
    std::string packageManagerStr = std::string(enums::to_string(packageManager));
    return std::find(metadata.supportedPackageManagers.begin(),
                     metadata.supportedPackageManagers.end(),
                     packageManagerStr) != metadata.supportedPackageManagers.end();
}

bool TemplateDiscovery::checkPlatformSupport(const TemplateMetadata& metadata) {
    (void)metadata;  // TODO: Implement platform-specific requirement checks
    // For now, assume all templates support current platform
    // In a real implementation, this would check platform-specific requirements
    return true;
}

int TemplateDiscovery::calculateCompatibilityScore(const TemplateMetadata& metadata,
                                                   const CliOptions& options) {
    int score = 100;

    // Reduce score for experimental templates
    if (metadata.isExperimental)
        score -= 20;

    // Reduce score for deprecated templates
    if (metadata.isDeprecated)
        score -= 50;

    // Check build system compatibility
    if (!validateBuildSystemCompatibility(metadata, options.buildSystem)) {
        score -= 30;
    }

    // Check package manager compatibility
    if (!validatePackageManagerCompatibility(metadata, options.packageManager)) {
        score -= 20;
    }

    return std::max(0, score);
}

// Template utility functions
namespace template_utils {

bool matchesQuery(const TemplateMetadata& metadata, const std::string& query) {
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    // Check name
    std::string lowerName = metadata.name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    if (lowerName.find(lowerQuery) != std::string::npos) {
        return true;
    }

    // Check display name
    std::string lowerDisplayName = metadata.displayName;
    std::transform(lowerDisplayName.begin(), lowerDisplayName.end(), lowerDisplayName.begin(),
                   ::tolower);
    if (lowerDisplayName.find(lowerQuery) != std::string::npos) {
        return true;
    }

    // Check description
    std::string lowerDescription = metadata.description;
    std::transform(lowerDescription.begin(), lowerDescription.end(), lowerDescription.begin(),
                   ::tolower);
    if (lowerDescription.find(lowerQuery) != std::string::npos) {
        return true;
    }

    // Check tags
    for (const auto& tag : metadata.tags) {
        std::string lowerTag = tag;
        std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
        if (lowerTag.find(lowerQuery) != std::string::npos) {
            return true;
        }
    }

    return false;
}

std::vector<TemplateMetadata> filterTemplates(const std::vector<TemplateMetadata>& templates,
                                              std::function<bool(const TemplateMetadata&)> filter) {
    std::vector<TemplateMetadata> filtered;

    for (const auto& template_meta : templates) {
        if (filter(template_meta)) {
            filtered.push_back(template_meta);
        }
    }

    return filtered;
}

std::vector<TemplateMetadata> sortTemplatesByRelevance(
        const std::vector<TemplateMetadata>& templates, const std::string& query) {
    std::vector<std::pair<TemplateMetadata, int>> templatesWithScores;

    for (const auto& template_meta : templates) {
        int relevanceScore = 0;

        // Exact name match gets highest score
        if (template_meta.name == query) {
            relevanceScore += 100;
        } else if (template_meta.name.find(query) != std::string::npos) {
            relevanceScore += 50;
        }

        // Display name match
        if (template_meta.displayName.find(query) != std::string::npos) {
            relevanceScore += 30;
        }

        // Description match
        if (template_meta.description.find(query) != std::string::npos) {
            relevanceScore += 20;
        }

        // Tag match
        for (const auto& tag : template_meta.tags) {
            if (tag.find(query) != std::string::npos) {
                relevanceScore += 10;
            }
        }

        templatesWithScores.emplace_back(template_meta, relevanceScore);
    }

    // Sort by relevance score (highest first)
    std::sort(templatesWithScores.begin(), templatesWithScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<TemplateMetadata> sorted;
    for (const auto& [template_meta, score] : templatesWithScores) {
        sorted.push_back(template_meta);
    }

    return sorted;
}

void printTemplateInfo(const TemplateMetadata& metadata) {
    using namespace utils;

    std::cout << TerminalUtils::colorize("📦 " + metadata.displayName, Color::BrightCyan) << "\n";
    std::cout << "   " << TerminalUtils::colorize("Name:", Color::BrightWhite) << " "
              << metadata.name << "\n";
    std::cout << "   " << TerminalUtils::colorize("Description:", Color::BrightWhite) << " "
              << metadata.description << "\n";
    std::cout << "   " << TerminalUtils::colorize("Category:", Color::BrightWhite) << " "
              << metadata.category << "\n";

    if (!metadata.tags.empty()) {
        std::cout << "   " << TerminalUtils::colorize("Tags:", Color::BrightWhite) << " ";
        for (size_t i = 0; i < metadata.tags.size(); ++i) {
            std::cout << TerminalUtils::colorize(metadata.tags[i], Color::BrightGreen);
            if (i < metadata.tags.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }

    std::cout << "   " << TerminalUtils::colorize("C++ Standard:", Color::BrightWhite) << " "
              << metadata.minCppStandard << "\n";

    if (metadata.isExperimental) {
        std::cout << "   " << TerminalUtils::colorize("⚠️  Experimental", Color::BrightYellow)
                  << "\n";
    }

    if (metadata.isDeprecated) {
        std::cout << "   " << TerminalUtils::colorize("❌ Deprecated", Color::BrightRed) << "\n";
    }

    std::cout << "\n";
}

void printTemplateList(const std::vector<TemplateMetadata>& templates) {
    if (templates.empty()) {
        std::cout << utils::TerminalUtils::colorize("No templates found.",
                                                    utils::Color::BrightYellow)
                  << "\n";
        return;
    }

    for (const auto& template_meta : templates) {
        printTemplateInfo(template_meta);
    }
}

}  // namespace template_utils

}  // namespace templates
