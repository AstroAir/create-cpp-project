#pragma once
#include "../cli/cli_parser.h"
#include "dependency_manager.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace utils {

// Enhanced dependency information with interactive features
struct InteractiveDependency {
    Dependency base;
    std::string category;
    std::vector<std::string> tags;
    std::string homepage;
    std::string documentation;
    int popularity = 0; // Popularity score (0-100)
    std::string lastUpdate;
    std::vector<std::string> alternatives;
    std::vector<std::string> useCases;
    bool isRecommended = false;
    std::string installationNotes;
    std::vector<std::string> examples;
};

// Package search and filtering
struct DependencySearchCriteria {
    std::string query;
    std::vector<std::string> categories;
    std::vector<std::string> tags;
    PackageManager preferredPackageManager = PackageManager::None;
    bool headerOnlyOnly = false;
    bool recommendedOnly = false;
    int minPopularity = 0;
    std::string cppStandardRequired;
};

// Dependency selection result
struct DependencySelection {
    std::vector<InteractiveDependency> selected;
    std::vector<std::string> conflicts;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> configuration;
    bool success = true;
};

// Interactive dependency manager
class InteractiveDependencyManager {
public:
    // Main interactive interface
    static DependencySelection runInteractiveDependencySelection(
        const CliOptions& options
    );

    // Dependency search and browsing
    static std::vector<InteractiveDependency> searchDependencies(
        const DependencySearchCriteria& criteria
    );

    static std::vector<InteractiveDependency> browseDependenciesByCategory(
        const std::string& category
    );

    static std::vector<InteractiveDependency> getRecommendedDependencies(
        TemplateType templateType
    );

    static std::vector<InteractiveDependency> getPopularDependencies(
        int limit = 20
    );

    // Interactive selection
    static std::vector<InteractiveDependency> selectDependenciesInteractively(
        const std::vector<InteractiveDependency>& available,
        const std::vector<InteractiveDependency>& preselected = {}
    );

    // Dependency configuration
    static bool configureDependencyInteractively(
        InteractiveDependency& dependency,
        const CliOptions& options
    );

    // Package manager selection and configuration
    static PackageManager selectPackageManagerInteractively(
        const std::vector<PackageManager>& available,
        TemplateType templateType
    );

    static bool configurePackageManagerInteractively(
        PackageManager packageManager,
        const CliOptions& options
    );

    // Testing framework selection
    static TestFramework selectTestingFrameworkInteractively(
        const std::vector<TestFramework>& available = {}
    );

    // Dependency validation and conflict resolution
    static DependencySelection validateAndResolveDependencies(
        const std::vector<InteractiveDependency>& selected,
        const CliOptions& options
    );

    // Information display
    static void showDependencyDetails(const InteractiveDependency& dependency);
    static void showDependencyComparison(
        const std::vector<InteractiveDependency>& dependencies
    );
    static void showPackageManagerComparison(
        const std::vector<PackageManager>& packageManagers
    );

    // Library database management
    static bool loadDependencyDatabase();
    static bool updateDependencyDatabase();
    static std::vector<std::string> getAvailableCategories();
    static std::vector<std::string> getAvailableTags();

    // Custom dependency management
    static bool addCustomDependency(const InteractiveDependency& dependency);
    static bool removeCustomDependency(const std::string& name);
    static std::vector<InteractiveDependency> getCustomDependencies();

    // Import/Export functionality
    static bool exportDependencyList(
        const std::vector<InteractiveDependency>& dependencies,
        const std::string& format, // "json", "yaml", "requirements"
        const std::string& filePath
    );

    static std::vector<InteractiveDependency> importDependencyList(
        const std::string& filePath,
        const std::string& format = "auto" // Auto-detect format
    );

    // Integration with existing systems
    static bool generatePackageManagerFiles(
        const std::vector<InteractiveDependency>& dependencies,
        PackageManager packageManager,
        const std::string& projectPath
    );

    static bool generateCMakeIntegration(
        const std::vector<InteractiveDependency>& dependencies,
        const std::string& projectPath
    );

    // Display functions
    static void showDependencySelectionSummary(const DependencySelection& selection);

private:
    // Internal data structures
    static std::vector<InteractiveDependency> s_dependencyDatabase;
    static std::unordered_map<std::string, std::vector<std::string>> s_categoryMap;
    static std::unordered_map<std::string, std::vector<std::string>> s_tagMap;
    static bool s_databaseLoaded;

    // Initialization and loading
    static void initializeDependencyDatabase();
    static void loadBuiltinDependencies();
    static void loadCustomDependencies();

    // Search and filtering helpers
    static bool matchesSearchCriteria(
        const InteractiveDependency& dependency,
        const DependencySearchCriteria& criteria
    );

    static std::vector<InteractiveDependency> filterDependencies(
        const std::vector<InteractiveDependency>& dependencies,
        const std::function<bool(const InteractiveDependency&)>& filter
    );

    static std::vector<InteractiveDependency> sortDependencies(
        const std::vector<InteractiveDependency>& dependencies,
        const std::string& sortBy = "popularity" // "name", "popularity", "category"
    );

    // Interactive UI helpers
    static void showDependencyList(
        const std::vector<InteractiveDependency>& dependencies,
        bool showDetails = false
    );

    static void showSearchInterface();
    static void showCategoryBrowser();
    static void showDependencyConfiguration(const InteractiveDependency& dependency);

    // Validation helpers
    static std::vector<std::string> checkDependencyConflicts(
        const std::vector<InteractiveDependency>& dependencies
    );

    static std::vector<std::string> checkVersionCompatibility(
        const std::vector<InteractiveDependency>& dependencies
    );

    static std::vector<std::string> checkPlatformCompatibility(
        const std::vector<InteractiveDependency>& dependencies,
        const std::string& platform
    );

    // Package manager helpers
    static std::vector<PackageManager> getAvailablePackageManagers();
    static std::vector<PackageManager> getRecommendedPackageManagers(
        TemplateType templateType
    );

    static std::string getPackageManagerDescription(PackageManager pm);
    static std::vector<std::string> getPackageManagerFeatures(PackageManager pm);

    // File generation helpers
    static std::string generateVcpkgJson(
        const std::vector<InteractiveDependency>& dependencies,
        const std::string& projectName
    );

    static std::string generateConanfile(
        const std::vector<InteractiveDependency>& dependencies
    );

    static std::string generateCMakeLists(
        const std::vector<InteractiveDependency>& dependencies
    );
};

// Dependency categories for organization
namespace categories {
    constexpr const char* UTILITY = "Utility";
    constexpr const char* NETWORKING = "Networking";
    constexpr const char* GUI = "GUI";
    constexpr const char* GRAPHICS = "Graphics";
    constexpr const char* AUDIO = "Audio";
    constexpr const char* DATABASE = "Database";
    constexpr const char* TESTING = "Testing";
    constexpr const char* LOGGING = "Logging";
    constexpr const char* SERIALIZATION = "Serialization";
    constexpr const char* CRYPTOGRAPHY = "Cryptography";
    constexpr const char* MATH = "Math";
    constexpr const char* GAME_DEV = "Game Development";
    constexpr const char* WEB = "Web";
    constexpr const char* EMBEDDED = "Embedded";
    constexpr const char* MACHINE_LEARNING = "Machine Learning";
}

// Common dependency tags
namespace tags {
    constexpr const char* HEADER_ONLY = "header-only";
    constexpr const char* CROSS_PLATFORM = "cross-platform";
    constexpr const char* HIGH_PERFORMANCE = "high-performance";
    constexpr const char* LIGHTWEIGHT = "lightweight";
    constexpr const char* MODERN_CPP = "modern-cpp";
    constexpr const char* STABLE = "stable";
    constexpr const char* POPULAR = "popular";
    constexpr const char* BEGINNER_FRIENDLY = "beginner-friendly";
}

} // namespace utils
