#include "project_profiles.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using namespace utils;

namespace config {

ProjectProfileManager& ProjectProfileManager::getInstance() {
    static ProjectProfileManager instance;
    return instance;
}

bool ProjectProfileManager::loadBuiltInProfiles() {
    spdlog::info("Loading built-in project profiles...");
    
    try {
        createWebDevelopmentProfiles();
        createGameDevelopmentProfiles();
        createEmbeddedProfiles();
        createLibraryProfiles();
        createResearchProfiles();
        createEnterpriseProfiles();
        
        spdlog::info("Loaded {} built-in profiles", profiles_.size());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to load built-in profiles: {}", e.what());
        return false;
    }
}

std::vector<std::string> ProjectProfileManager::listProfiles() const {
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    
    for (const auto& [name, profile] : profiles_) {
        names.push_back(name);
    }
    
    std::sort(names.begin(), names.end());
    return names;
}

std::vector<std::string> ProjectProfileManager::listProfilesByCategory(const std::string& category) const {
    std::vector<std::string> names;
    
    for (const auto& [name, profile] : profiles_) {
        if (profile.info.category == category) {
            names.push_back(name);
        }
    }
    
    std::sort(names.begin(), names.end());
    return names;
}

std::optional<ProjectProfile> ProjectProfileManager::getProfile(const std::string& name) const {
    auto it = profiles_.find(name);
    if (it != profiles_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ProfileInfo> ProjectProfileManager::getProfileInfos() const {
    std::vector<ProfileInfo> infos;
    infos.reserve(profiles_.size());
    
    for (const auto& [name, profile] : profiles_) {
        infos.push_back(profile.info);
    }
    
    return infos;
}

CliOptions ProjectProfileManager::applyProfile(const std::string& profileName, const CliOptions& baseOptions) const {
    auto profile = getProfile(profileName);
    if (!profile) {
        spdlog::warn("Profile '{}' not found, using base options", profileName);
        return baseOptions;
    }
    
    // Record usage
    const_cast<ProjectProfileManager*>(this)->recordProfileUsage(profileName);
    
    // Start with profile options
    CliOptions result = profile->options;
    
    // Override with any non-default values from base options
    if (!baseOptions.projectName.empty()) {
        result.projectName = baseOptions.projectName;
    }
    
    // Merge editor and CI options
    if (!baseOptions.editorOptions.empty()) {
        result.editorOptions = baseOptions.editorOptions;
    }
    if (!baseOptions.ciOptions.empty()) {
        result.ciOptions = baseOptions.ciOptions;
    }
    
    spdlog::info("Applied profile '{}' to project configuration", profileName);
    return result;
}

void ProjectProfileManager::recordProfileUsage(const std::string& profileName) {
    usageStats_[profileName]++;
}

std::map<std::string, int> ProjectProfileManager::getProfileUsageStats() const {
    return usageStats_;
}

void ProjectProfileManager::createWebDevelopmentProfiles() {
    // REST API Service Profile
    {
        ProjectProfile profile;
        profile.info = {
            "rest-api",
            "Modern REST API service with database integration",
            "Web Development",
            {"api", "rest", "web", "service", "backend"},
            "CPP-Scaffold Team",
            "1.0.0",
            true
        };
        
        profile.options.templateType = TemplateType::WebService;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::Vcpkg;
        profile.options.includeTests = true;
        profile.options.testFramework = TestFramework::GTest;
        profile.options.includeDocumentation = true;
        profile.options.includeCodeStyleTools = true;
        profile.options.initGit = true;
        
        profile.recommendedDependencies = {
            "nlohmann-json", "spdlog", "fmt", "httplib", "sqlite3"
        };
        
        profile.setupInstructions = R"(
1. Install vcpkg and required dependencies
2. Configure database connection in config/config.json
3. Build with: cmake --build build --config Release
4. Run tests with: ctest --test-dir build
5. Start server with: ./build/bin/your-project
)";
        
        profiles_["rest-api"] = profile;
    }
    
    // Microservice Profile
    {
        ProjectProfile profile;
        profile.info = {
            "microservice",
            "Lightweight microservice with Docker support",
            "Web Development", 
            {"microservice", "docker", "api", "cloud"},
            "CPP-Scaffold Team",
            "1.0.0",
            true
        };
        
        profile.options.templateType = TemplateType::WebService;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::Conan;
        profile.options.includeTests = true;
        profile.options.testFramework = TestFramework::Catch2;
        profile.options.includeDocumentation = true;
        profile.options.ciOptions = {CiSystem::GitHub, CiSystem::GitLab};
        
        profile.recommendedDependencies = {
            "boost", "nlohmann-json", "spdlog", "prometheus-cpp"
        };
        
        profiles_["microservice"] = profile;
    }
}

void ProjectProfileManager::createGameDevelopmentProfiles() {
    // Game Engine Profile
    {
        ProjectProfile profile;
        profile.info = {
            "game-engine",
            "2D/3D game engine with modern graphics APIs",
            "Game Development",
            {"game", "engine", "graphics", "opengl", "vulkan"},
            "CPP-Scaffold Team", 
            "1.0.0",
            true
        };
        
        profile.options.templateType = TemplateType::GameEngine;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::Vcpkg;
        profile.options.includeTests = true;
        profile.options.testFramework = TestFramework::GTest;
        
        profile.recommendedDependencies = {
            "glfw3", "glm", "assimp", "opengl", "vulkan"
        };
        
        profiles_["game-engine"] = profile;
    }
}

void ProjectProfileManager::createEmbeddedProfiles() {
    // IoT Device Profile
    {
        ProjectProfile profile;
        profile.info = {
            "iot-device",
            "IoT device firmware with connectivity features",
            "Embedded Systems",
            {"iot", "embedded", "firmware", "connectivity"},
            "CPP-Scaffold Team",
            "1.0.0", 
            true
        };
        
        profile.options.templateType = TemplateType::Embedded;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::None;
        profile.options.includeTests = true;
        profile.options.testFramework = TestFramework::GTest;
        
        profile.recommendedDependencies = {
            "freertos", "lwip", "mbedtls"
        };
        
        profiles_["iot-device"] = profile;
    }
}

void ProjectProfileManager::createLibraryProfiles() {
    // Header-only Library Profile
    {
        ProjectProfile profile;
        profile.info = {
            "header-only-lib",
            "Modern header-only C++ library",
            "Libraries",
            {"library", "header-only", "template", "modern-cpp"},
            "CPP-Scaffold Team",
            "1.0.0",
            true
        };
        
        profile.options.templateType = TemplateType::HeaderOnlyLib;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::Vcpkg;
        profile.options.includeTests = true;
        profile.options.testFramework = TestFramework::Catch2;
        profile.options.includeDocumentation = true;
        profile.options.includeCodeStyleTools = true;
        
        profiles_["header-only-lib"] = profile;
    }
}

void ProjectProfileManager::createResearchProfiles() {
    // Research Project Profile
    {
        ProjectProfile profile;
        profile.info = {
            "research",
            "Academic research project with data analysis tools",
            "Research & Academic",
            {"research", "academic", "data", "analysis"},
            "CPP-Scaffold Team",
            "1.0.0",
            true
        };
        
        profile.options.templateType = TemplateType::Console;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::Vcpkg;
        profile.options.includeTests = true;
        profile.options.includeDocumentation = true;
        
        profile.recommendedDependencies = {
            "eigen3", "boost", "matplotlib-cpp", "csv-parser"
        };
        
        profiles_["research"] = profile;
    }
}

void ProjectProfileManager::createEnterpriseProfiles() {
    // Enterprise Application Profile
    {
        ProjectProfile profile;
        profile.info = {
            "enterprise",
            "Large-scale enterprise application",
            "Enterprise",
            {"enterprise", "scalable", "distributed", "performance"},
            "CPP-Scaffold Team",
            "1.0.0",
            true
        };

        profile.options.templateType = TemplateType::Console;
        profile.options.buildSystem = BuildSystem::CMake;
        profile.options.packageManager = PackageManager::Conan;
        profile.options.includeTests = true;
        profile.options.testFramework = TestFramework::GTest;
        profile.options.includeDocumentation = true;
        profile.options.includeCodeStyleTools = true;
        profile.options.ciOptions = {CiSystem::GitHub, CiSystem::GitLab};

        profile.recommendedDependencies = {
            "boost", "protobuf", "grpc", "redis-plus-plus", "postgresql"
        };

        profiles_["enterprise"] = profile;
    }
}

} // namespace config
