#pragma once

#include "enhanced_terminal.h"
#include "../cli/types/cli_options.h"
#include <string>
#include <vector>
#include <functional>

namespace utils {

// Enhanced interactive project creation wizard
class EnhancedWizard {
public:
    static EnhancedWizard& getInstance();
    
    // Main wizard entry point
    CliOptions runInteractiveWizard();
    
    // Wizard steps
    CliOptions runQuickSetup();
    CliOptions runAdvancedSetup();
    CliOptions runGuidedSetup();
    
    // Individual setup steps
    std::string promptProjectName();
    TemplateType promptTemplateType();
    BuildSystem promptBuildSystem(TemplateType templateType);
    PackageManager promptPackageManager();
    TestFramework promptTestFramework();
    bool promptIncludeTests();
    bool promptIncludeDocumentation();
    bool promptInitGit();
    std::string promptGitRemote();
    
    // Advanced options
    std::string promptNetworkLibrary();
    std::string promptGuiFramework();
    std::vector<std::string> promptAdditionalFeatures();
    
    // Configuration and validation
    void showConfigurationSummary(const CliOptions& options);
    bool confirmConfiguration(const CliOptions& options);
    CliOptions validateAndFixConfiguration(const CliOptions& options);
    
    // Help and guidance
    void showTemplateHelp();
    void showBuildSystemHelp();
    void showPackageManagerHelp();
    void showBestPracticesAdvice(const CliOptions& options);
    
private:
    EnhancedWizard() = default;
    
    // Helper methods
    template<typename T>
    T promptFromEnum(const std::string& prompt, 
                     const std::vector<std::pair<T, std::string>>& options,
                     const std::string& helpText = "");
    
    std::string promptString(const std::string& prompt, 
                           const std::string& defaultValue = "",
                           const std::function<bool(const std::string&)>& validator = nullptr);
    
    bool promptYesNo(const std::string& prompt, bool defaultValue = true);
    
    int promptChoice(const std::string& prompt, 
                    const std::vector<std::string>& choices,
                    const std::string& helpText = "");
    
    // Validation helpers
    bool isValidProjectName(const std::string& name);
    bool isCompatibleCombination(TemplateType templateType, BuildSystem buildSystem);
    std::vector<std::string> getRecommendations(const CliOptions& options);
    
    // Display helpers
    void showWelcomeMessage();
    void showStepHeader(const std::string& stepName, int stepNumber, int totalSteps);
    void showProgress(int currentStep, int totalSteps);
};

// Template specializations for enum prompting
template<>
TemplateType EnhancedWizard::promptFromEnum<TemplateType>(
    const std::string& prompt, 
    const std::vector<std::pair<TemplateType, std::string>>& options,
    const std::string& helpText);

template<>
BuildSystem EnhancedWizard::promptFromEnum<BuildSystem>(
    const std::string& prompt, 
    const std::vector<std::pair<BuildSystem, std::string>>& options,
    const std::string& helpText);

template<>
PackageManager EnhancedWizard::promptFromEnum<PackageManager>(
    const std::string& prompt, 
    const std::vector<std::pair<PackageManager, std::string>>& options,
    const std::string& helpText);

template<>
TestFramework EnhancedWizard::promptFromEnum<TestFramework>(
    const std::string& prompt, 
    const std::vector<std::pair<TestFramework, std::string>>& options,
    const std::string& helpText);

} // namespace utils
