#pragma once

#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>

namespace templates {

// Template metadata for discovery and validation
struct TemplateMetadata {
    std::string name;
    std::string displayName;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    std::vector<std::string> supportedBuildSystems;
    std::vector<std::string> supportedPackageManagers;
    std::vector<std::string> requiredDependencies;
    std::vector<std::string> optionalDependencies;
    std::string minCppStandard;
    bool isExperimental;
    bool isDeprecated;
    std::string version;
    std::string author;
    std::string documentation;
    std::string repository;
};

// Template compatibility information
struct TemplateCompatibility {
    bool isCompatible;
    std::vector<std::string> issues;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestions;
    int compatibilityScore; // 0-100
};

// Template validation result
struct TemplateValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestions;
};

// Template discovery and management system
class TemplateDiscovery {
public:
    static TemplateDiscovery& getInstance();

    // Template discovery
    std::vector<TemplateMetadata> discoverTemplates();
    std::vector<TemplateMetadata> getAvailableTemplates();
    std::vector<TemplateMetadata> getTemplatesByCategory(const std::string& category);
    std::vector<TemplateMetadata> getTemplatesByTag(const std::string& tag);
    std::vector<TemplateMetadata> searchTemplates(const std::string& query);

    // Template metadata
    std::optional<TemplateMetadata> getTemplateMetadata(const std::string& templateName);
    std::optional<TemplateMetadata> getTemplateMetadata(TemplateType templateType);

    // Template validation
    TemplateValidationResult validateTemplate(const std::string& templateName);
    TemplateValidationResult validateTemplate(TemplateType templateType);
    TemplateValidationResult validateTemplateWithOptions(const std::string& templateName, const CliOptions& options);

    // Template compatibility
    TemplateCompatibility checkCompatibility(const std::string& templateName, const CliOptions& options);
    TemplateCompatibility checkSystemCompatibility(const std::string& templateName);

    // Template recommendations
    std::vector<std::string> getRecommendedTemplates(const CliOptions& options);
    std::vector<std::string> getSimilarTemplates(const std::string& templateName);

    // Template categories and tags
    std::vector<std::string> getAvailableCategories();
    std::vector<std::string> getAvailableTags();
    std::map<std::string, std::vector<std::string>> getTemplatesByCategories();

    // Template statistics
    std::map<std::string, int> getTemplateUsageStats();
    std::vector<std::string> getMostPopularTemplates(int count = 10);
    std::vector<std::string> getRecentlyUsedTemplates(int count = 5);

    // Custom template management
    bool registerCustomTemplate(const TemplateMetadata& metadata);
    bool unregisterCustomTemplate(const std::string& templateName);
    std::vector<TemplateMetadata> getCustomTemplates();

    // Template updates and versioning
    bool isTemplateUpdateAvailable(const std::string& templateName);
    std::string getLatestTemplateVersion(const std::string& templateName);
    bool updateTemplate(const std::string& templateName);

private:
    TemplateDiscovery() = default;

    // Internal template registry
    std::map<std::string, TemplateMetadata> templateRegistry_;
    std::map<std::string, int> usageStats_;
    std::vector<std::string> recentlyUsed_;

    // Template metadata initialization
    void initializeBuiltinTemplates();
    void loadCustomTemplates();
    void loadTemplateUsageStats();

    // Template validation helpers
    bool validateBuildSystemCompatibility(const TemplateMetadata& metadata, BuildSystem buildSystem);
    bool validatePackageManagerCompatibility(const TemplateMetadata& metadata, PackageManager packageManager);
    bool validateDependencies(const TemplateMetadata& metadata);
    bool validateCppStandard(const TemplateMetadata& metadata, const std::string& requiredStandard);

    // System compatibility checks
    bool checkPlatformSupport(const TemplateMetadata& metadata);
    bool checkToolchainSupport(const TemplateMetadata& metadata);
    bool checkDependencyAvailability(const TemplateMetadata& metadata);

    // Recommendation algorithms
    int calculateCompatibilityScore(const TemplateMetadata& metadata, const CliOptions& options);
    std::vector<std::string> findSimilarTemplatesByTags(const std::string& templateName);
    std::vector<std::string> findSimilarTemplatesByCategory(const std::string& templateName);

    // Usage tracking
    void recordTemplateUsage(const std::string& templateName);
    void updateRecentlyUsed(const std::string& templateName);
    void saveUsageStats();
};

// Template filter and search utilities
namespace template_utils {
    // Filter functions
    std::function<bool(const TemplateMetadata&)> createCategoryFilter(const std::string& category);
    std::function<bool(const TemplateMetadata&)> createTagFilter(const std::string& tag);
    std::function<bool(const TemplateMetadata&)> createBuildSystemFilter(BuildSystem buildSystem);
    std::function<bool(const TemplateMetadata&)> createPackageManagerFilter(PackageManager packageManager);
    std::function<bool(const TemplateMetadata&)> createExperimentalFilter(bool includeExperimental);

    // Search functions
    bool matchesQuery(const TemplateMetadata& metadata, const std::string& query);
    std::vector<TemplateMetadata> filterTemplates(const std::vector<TemplateMetadata>& templates,
                                                  std::function<bool(const TemplateMetadata&)> filter);
    std::vector<TemplateMetadata> sortTemplatesByRelevance(const std::vector<TemplateMetadata>& templates,
                                                           const std::string& query);

    // Display utilities
    void printTemplateInfo(const TemplateMetadata& metadata);
    void printTemplateList(const std::vector<TemplateMetadata>& templates);
    void printCompatibilityReport(const TemplateCompatibility& compatibility);
    void printValidationReport(const TemplateValidationResult& validation);
}

} // namespace templates
