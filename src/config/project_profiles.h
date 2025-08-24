#pragma once

#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace config {

// Profile metadata
struct ProfileInfo {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    std::string author;
    std::string version;
    bool isBuiltIn = true;
};

// Project profile containing complete configuration
struct ProjectProfile {
    ProfileInfo info;
    CliOptions options;
    
    // Additional profile-specific settings
    std::map<std::string, std::string> customSettings;
    std::vector<std::string> recommendedDependencies;
    std::vector<std::string> optionalDependencies;
    std::string setupInstructions;
    std::string usageGuide;
};

// Profile validation result
struct ProfileValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestions;
};

class ProjectProfileManager {
public:
    static ProjectProfileManager& getInstance();
    
    // Profile management
    bool loadBuiltInProfiles();
    bool loadCustomProfiles(const std::string& profilesDir = "");
    bool saveProfile(const ProjectProfile& profile, const std::string& path = "");
    bool deleteProfile(const std::string& profileName);
    
    // Profile access
    std::vector<std::string> listProfiles() const;
    std::vector<std::string> listProfilesByCategory(const std::string& category) const;
    std::vector<std::string> searchProfiles(const std::string& query) const;
    std::optional<ProjectProfile> getProfile(const std::string& name) const;
    std::vector<ProfileInfo> getProfileInfos() const;
    
    // Profile operations
    ProfileValidationResult validateProfile(const ProjectProfile& profile) const;
    ProjectProfile createProfileFromOptions(const CliOptions& options, const ProfileInfo& info) const;
    CliOptions applyProfile(const std::string& profileName, const CliOptions& baseOptions = {}) const;
    
    // Profile recommendations
    std::vector<std::string> recommendProfiles(const CliOptions& options) const;
    std::vector<std::string> getSimilarProfiles(const std::string& profileName) const;
    
    // Interactive profile creation
    ProjectProfile createProfileInteractively() const;
    bool editProfileInteractively(const std::string& profileName);
    
    // Profile import/export
    bool exportProfile(const std::string& profileName, const std::string& filePath) const;
    bool importProfile(const std::string& filePath);
    bool exportAllProfiles(const std::string& directory) const;
    
    // Profile statistics and analytics
    std::map<std::string, int> getProfileUsageStats() const;
    void recordProfileUsage(const std::string& profileName);
    
private:
    ProjectProfileManager() = default;
    
    std::map<std::string, ProjectProfile> profiles_;
    std::map<std::string, int> usageStats_;
    
    // Built-in profile creation
    void createWebDevelopmentProfiles();
    void createGameDevelopmentProfiles();
    void createEmbeddedProfiles();
    void createLibraryProfiles();
    void createResearchProfiles();
    void createEnterpriseProfiles();
    
    // Helper methods
    bool validateProfileName(const std::string& name) const;
    std::string generateProfileId(const std::string& name) const;
    bool isProfileCompatible(const ProjectProfile& profile, const CliOptions& options) const;
    std::string getDefaultProfilesDirectory() const;
    
    // Profile serialization
    std::string serializeProfile(const ProjectProfile& profile) const;
    std::optional<ProjectProfile> deserializeProfile(const std::string& data) const;
};

// Built-in profile definitions
namespace profiles {
    // Web Development
    extern const ProjectProfile REST_API_SERVICE;
    extern const ProjectProfile MICROSERVICE;
    extern const ProjectProfile WEB_BACKEND;
    extern const ProjectProfile GRAPHQL_SERVER;
    
    // Game Development  
    extern const ProjectProfile GAME_ENGINE;
    extern const ProjectProfile INDIE_GAME;
    extern const ProjectProfile MOBILE_GAME;
    extern const ProjectProfile VR_APPLICATION;
    
    // Embedded Systems
    extern const ProjectProfile IOT_DEVICE;
    extern const ProjectProfile MICROCONTROLLER;
    extern const ProjectProfile REAL_TIME_SYSTEM;
    extern const ProjectProfile AUTOMOTIVE;
    
    // Libraries
    extern const ProjectProfile HEADER_ONLY_LIBRARY;
    extern const ProjectProfile SHARED_LIBRARY;
    extern const ProjectProfile STATIC_LIBRARY;
    extern const ProjectProfile CROSS_PLATFORM_LIB;
    
    // Research & Academic
    extern const ProjectProfile RESEARCH_PROJECT;
    extern const ProjectProfile ALGORITHM_IMPLEMENTATION;
    extern const ProjectProfile DATA_ANALYSIS;
    extern const ProjectProfile MACHINE_LEARNING;
    
    // Enterprise
    extern const ProjectProfile ENTERPRISE_APPLICATION;
    extern const ProjectProfile DISTRIBUTED_SYSTEM;
    extern const ProjectProfile HIGH_PERFORMANCE_COMPUTING;
    extern const ProjectProfile FINANCIAL_SYSTEM;
}

} // namespace config
