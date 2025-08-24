#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <optional>
#include <functional>

namespace utils {

// Framework and library integration system
class FrameworkIntegration {
public:
    // Framework types
    enum class Framework {
        Qt,
        SFML,
        Boost,
        GoogleTest,
        Catch2,
        OpenCV,
        Eigen,
        FLTK,
        GTK,
        wxWidgets,
        Dear_ImGui,
        GLFW,
        OpenGL,
        Vulkan,
        SDL2,
        Allegro,
        POCO,
        Asio,
        gRPC,
        Protobuf,
        JSON_nlohmann,
        fmt,
        spdlog,
        CLI11,
        Doctest
    };
    
    // Package manager types
    enum class PackageManager {
        vcpkg,
        Conan,
        Hunter,
        CPM,
        FetchContent,
        System
    };
    
    // Framework information
    struct FrameworkInfo {
        Framework framework;
        std::string name;
        std::string description;
        std::string version;
        std::vector<std::string> dependencies;
        std::map<PackageManager, std::string> packageNames;
        std::vector<std::string> cmakeTargets;
        std::vector<std::string> includeDirectories;
        std::vector<std::string> libraryDirectories;
        std::vector<std::string> libraries;
        std::map<std::string, std::string> cmakeVariables;
        std::vector<std::string> compilerFlags;
        std::vector<std::string> linkerFlags;
        bool requiresSpecialSetup;
        std::function<bool(const std::filesystem::path&)> setupFunction;
    };
    
    // Framework detection and installation
    static bool isFrameworkAvailable(Framework framework, PackageManager packageManager = PackageManager::vcpkg);
    static bool installFramework(Framework framework, const std::filesystem::path& projectPath, PackageManager packageManager = PackageManager::vcpkg);
    static bool configureFramework(Framework framework, const std::filesystem::path& projectPath);
    
    // Framework information
    static std::optional<FrameworkInfo> getFrameworkInfo(Framework framework);
    static std::vector<Framework> listAvailableFrameworks();
    static std::vector<Framework> getCompatibleFrameworks(Framework baseFramework);
    
    // CMake integration
    static std::string generateCMakeConfig(const std::vector<Framework>& frameworks);
    static std::string generateFindPackageCommands(const std::vector<Framework>& frameworks);
    static std::string generateTargetLinkLibraries(const std::vector<Framework>& frameworks, const std::string& targetName);
    
    // Package manager integration
    static bool setupPackageManager(PackageManager packageManager, const std::filesystem::path& projectPath);
    static std::string generatePackageManagerConfig(PackageManager packageManager, const std::vector<Framework>& frameworks);
    
    // Template integration
    static bool integrateWithTemplate(const std::vector<Framework>& frameworks, const std::filesystem::path& projectPath);
    static std::vector<std::string> generateTemplateFiles(Framework framework, const std::filesystem::path& projectPath);
    
private:
    // Framework registry
    static std::map<Framework, FrameworkInfo> s_frameworkRegistry;
    static void initializeFrameworkRegistry();
    
    // Package manager helpers
    static bool setupVcpkg(const std::filesystem::path& projectPath, const std::vector<Framework>& frameworks);
    static bool setupConan(const std::filesystem::path& projectPath, const std::vector<Framework>& frameworks);
    static bool setupCPM(const std::filesystem::path& projectPath, const std::vector<Framework>& frameworks);
    
    // Framework-specific setup functions
    static bool setupQt(const std::filesystem::path& projectPath);
    static bool setupSFML(const std::filesystem::path& projectPath);
    static bool setupBoost(const std::filesystem::path& projectPath);
    static bool setupGoogleTest(const std::filesystem::path& projectPath);
    static bool setupOpenCV(const std::filesystem::path& projectPath);
    static bool setupImGui(const std::filesystem::path& projectPath);
    static bool setupOpenGL(const std::filesystem::path& projectPath);

    // Additional GUI framework setup functions
    static bool setupGTK(const std::filesystem::path& projectPath);
    static bool setupFLTK(const std::filesystem::path& projectPath);
    static bool setupWxWidgets(const std::filesystem::path& projectPath);

    // Game development framework setup functions
    static bool setupSDL2(const std::filesystem::path& projectPath);
    static bool setupAllegro(const std::filesystem::path& projectPath);

    // Graphics library setup functions
    static bool setupGLFW(const std::filesystem::path& projectPath);
    static bool setupVulkan(const std::filesystem::path& projectPath);
    
    // Utility functions
    static std::string frameworkToString(Framework framework);
    static std::optional<Framework> stringToFramework(const std::string& name);
    static std::string packageManagerToString(PackageManager pm);
    static std::optional<PackageManager> stringToPackageManager(const std::string& name);
};

// Dependency resolver for frameworks
class DependencyResolver {
public:
    // Dependency resolution
    static std::vector<FrameworkIntegration::Framework> resolveDependencies(const std::vector<FrameworkIntegration::Framework>& requestedFrameworks);
    static bool hasCircularDependencies(const std::vector<FrameworkIntegration::Framework>& frameworks);
    static std::vector<FrameworkIntegration::Framework> getInstallationOrder(const std::vector<FrameworkIntegration::Framework>& frameworks);
    
    // Conflict detection
    static std::vector<std::pair<FrameworkIntegration::Framework, FrameworkIntegration::Framework>> detectConflicts(const std::vector<FrameworkIntegration::Framework>& frameworks);
    static bool areFrameworksCompatible(FrameworkIntegration::Framework framework1, FrameworkIntegration::Framework framework2);
    
    // Version management
    static std::string getRecommendedVersion(FrameworkIntegration::Framework framework);
    static bool isVersionCompatible(FrameworkIntegration::Framework framework, const std::string& version);
    
private:
    // Dependency graph
    static std::map<FrameworkIntegration::Framework, std::vector<FrameworkIntegration::Framework>> s_dependencyGraph;
    static std::map<std::pair<FrameworkIntegration::Framework, FrameworkIntegration::Framework>, bool> s_compatibilityMatrix;
    
    // Graph algorithms
    static bool hasCycleDFS(FrameworkIntegration::Framework framework, 
                           std::map<FrameworkIntegration::Framework, int>& visited,
                           const std::vector<FrameworkIntegration::Framework>& frameworks);
    static void topologicalSortDFS(FrameworkIntegration::Framework framework,
                                  std::map<FrameworkIntegration::Framework, bool>& visited,
                                  std::vector<FrameworkIntegration::Framework>& result,
                                  const std::vector<FrameworkIntegration::Framework>& frameworks);
};

// Framework template generator
class FrameworkTemplateGenerator {
public:
    // Template generation
    static std::string generateMainCpp(FrameworkIntegration::Framework framework);
    static std::string generateHeaderFile(FrameworkIntegration::Framework framework, const std::string& className);
    static std::string generateSourceFile(FrameworkIntegration::Framework framework, const std::string& className);
    static std::string generateCMakeLists(const std::vector<FrameworkIntegration::Framework>& frameworks, const std::string& projectName);
    
    // Example code generation
    static std::string generateHelloWorldExample(FrameworkIntegration::Framework framework);
    static std::string generateBasicWindowExample(FrameworkIntegration::Framework framework);
    static std::string generateTestExample(FrameworkIntegration::Framework framework);
    
    // Documentation generation
    static std::string generateReadme(const std::vector<FrameworkIntegration::Framework>& frameworks, const std::string& projectName);
    static std::string generateBuildInstructions(const std::vector<FrameworkIntegration::Framework>& frameworks);
    static std::string generateUsageExamples(FrameworkIntegration::Framework framework);
    
private:
    // Template helpers
    static std::string getFrameworkIncludes(FrameworkIntegration::Framework framework);
    static std::string getFrameworkNamespace(FrameworkIntegration::Framework framework);
    static std::string getFrameworkInitialization(FrameworkIntegration::Framework framework);
    static std::string getFrameworkCleanup(FrameworkIntegration::Framework framework);
};

// Interactive framework selector
class FrameworkSelector {
public:
    // Interactive selection
    static std::vector<FrameworkIntegration::Framework> selectFrameworksInteractively();
    static FrameworkIntegration::PackageManager selectPackageManagerInteractively();
    
    // Framework categories
    enum class Category {
        GUI,
        Graphics,
        Audio,
        Networking,
        Testing,
        Utility,
        Math,
        Database,
        Serialization,
        Logging
    };
    
    static std::vector<FrameworkIntegration::Framework> getFrameworksByCategory(Category category);
    static std::vector<FrameworkIntegration::Framework> selectByCategory();
    
    // Recommendation system
    static std::vector<FrameworkIntegration::Framework> recommendFrameworks(const std::string& projectType);
    static std::vector<FrameworkIntegration::Framework> getPopularFrameworks();
    static std::vector<FrameworkIntegration::Framework> getFrameworksForBeginners();
    
private:
    // Interactive helpers
    static void displayFrameworkInfo(FrameworkIntegration::Framework framework);
    static bool confirmFrameworkSelection(const std::vector<FrameworkIntegration::Framework>& frameworks);
    static void showDependencyTree(const std::vector<FrameworkIntegration::Framework>& frameworks);
    
    // Category mappings
    static std::map<Category, std::vector<FrameworkIntegration::Framework>> s_categoryMap;
    static std::map<std::string, std::vector<FrameworkIntegration::Framework>> s_projectTypeRecommendations;
};

// Framework version manager
class FrameworkVersionManager {
public:
    // Version management
    static std::vector<std::string> getAvailableVersions(FrameworkIntegration::Framework framework);
    static std::string getLatestVersion(FrameworkIntegration::Framework framework);
    static std::string getStableVersion(FrameworkIntegration::Framework framework);
    
    // Version comparison
    static int compareVersions(const std::string& version1, const std::string& version2);
    static bool isVersionNewer(const std::string& version1, const std::string& version2);
    static bool isVersionInRange(const std::string& version, const std::string& minVersion, const std::string& maxVersion);
    
    // Version constraints
    static bool satisfiesConstraints(FrameworkIntegration::Framework framework, const std::string& version, const std::vector<std::string>& constraints);
    static std::string resolveVersionConstraints(FrameworkIntegration::Framework framework, const std::vector<std::string>& constraints);
    
private:
    // Version parsing
    static std::vector<int> parseVersion(const std::string& version);
    static bool isValidVersion(const std::string& version);
    
    // Version database
    static std::map<FrameworkIntegration::Framework, std::vector<std::string>> s_versionDatabase;
    static void initializeVersionDatabase();
};

} // namespace utils
