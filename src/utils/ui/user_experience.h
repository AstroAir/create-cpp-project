#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "../../cli/cli_parser.h"


namespace utils {

// User experience level for adaptive interfaces
enum class ExperienceLevel {
    Beginner,      // New to C++ or project setup
    Intermediate,  // Some experience with C++ projects
    Advanced,      // Experienced developer
    Expert         // Power user who wants minimal guidance
};

// Guidance context for contextual help
enum class GuidanceContext {
    ProjectCreation,
    TemplateSelection,
    BuildSystemSetup,
    DependencyManagement,
    Testing,
    Documentation,
    Deployment,
    Troubleshooting
};

// Tutorial step definition
struct TutorialStep {
    std::string title;
    std::string description;
    std::vector<std::string> instructions;
    std::vector<std::string> examples;
    std::vector<std::string> tips;
    std::vector<std::string> commonMistakes;
    std::function<bool()> validator;  // Optional validation function
    bool isOptional = false;
};

// Tutorial definition
struct Tutorial {
    std::string name;
    std::string description;
    std::string category;
    ExperienceLevel targetLevel;
    std::vector<TutorialStep> steps;
    std::vector<std::string> prerequisites;
    std::vector<std::string> learningOutcomes;
    int estimatedTimeMinutes;
};

// Auto-completion suggestion
struct CompletionSuggestion {
    std::string text;
    std::string description;
    std::string category;
    int priority = 0;
    bool isExact = false;
    std::vector<std::string> aliases;
};

// Contextual help entry
struct HelpEntry {
    std::string topic;
    std::string shortDescription;
    std::string detailedDescription;
    std::vector<std::string> examples;
    std::vector<std::string> relatedTopics;
    std::vector<std::string> troubleshooting;
    GuidanceContext context;
};

// User experience enhancement manager
class UserExperienceManager {
   public:
    static UserExperienceManager& getInstance();

    // Experience level management
    ExperienceLevel detectExperienceLevel();
    void setExperienceLevel(ExperienceLevel level);
    ExperienceLevel getExperienceLevel();
    bool shouldShowGuidance(GuidanceContext context);

    // Guided setup workflows
    CliOptions runGuidedSetup();
    CliOptions runQuickSetup();
    CliOptions runAdvancedSetup();
    bool runFirstTimeSetup();

    // Tutorial system
    std::vector<Tutorial> getAvailableTutorials(ExperienceLevel level = ExperienceLevel::Beginner);
    std::optional<Tutorial> getTutorial(const std::string& name);
    bool runTutorial(const std::string& name);
    bool runInteractiveTutorial(const Tutorial& tutorial);

    // Auto-completion system
    std::vector<CompletionSuggestion> getCompletions(const std::string& input,
                                                     GuidanceContext context);
    std::vector<CompletionSuggestion> getCommandCompletions(const std::string& partial);
    std::vector<CompletionSuggestion> getArgumentCompletions(const std::string& command,
                                                             const std::string& partial);
    std::vector<CompletionSuggestion> getValueCompletions(const std::string& argument,
                                                          const std::string& partial);

    // Contextual help system
    void showContextualHelp(GuidanceContext context);
    void showTopicHelp(const std::string& topic);
    std::vector<HelpEntry> searchHelp(const std::string& query);
    void showQuickReference();
    void showTroubleshootingGuide();

    // Examples and best practices
    void showExamples(const std::string& category = "");
    void showBestPractices(TemplateType templateType);
    void showCommonPatterns();
    void showAntiPatterns();

    // Next steps and recommendations
    std::vector<std::string> getNextSteps(const CliOptions& options);
    std::vector<std::string> getRecommendations(const CliOptions& options);
    void showPostCreationGuide(const CliOptions& options);
    void showDevelopmentWorkflow(const CliOptions& options);

    // Progress tracking and achievements
    void trackProgress(const std::string& action, const CliOptions& options);
    std::vector<std::string> getAchievements();
    void showProgressSummary();
    bool hasCompletedAction(const std::string& action);

    // Adaptive interface
    void adaptInterfaceToUser();
    bool shouldShowAdvancedOptions();
    bool shouldShowDetailedOutput();
    int getRecommendedVerbosityLevel();

    // Onboarding system
    bool isFirstTimeUser();
    void runOnboardingFlow();
    void showWelcomeMessage();
    void showFeatureHighlights();
    void collectUserPreferences();

    // Error handling and recovery
    void showErrorGuidance(const std::string& error, GuidanceContext context);
    std::vector<std::string> suggestErrorSolutions(const std::string& error);
    void showRecoveryOptions(const std::string& error);

    // Interactive assistance
    bool offerInteractiveHelp(const std::string& context);
    void startInteractiveAssistant();
    void showSmartSuggestions(const CliOptions& options);

    // Documentation integration
    void openDocumentation(const std::string& topic);
    void showInlineDocumentation(const std::string& topic);
    std::vector<std::string> getDocumentationLinks(const std::string& topic);

    // Feedback and improvement
    void collectFeedback(const std::string& context);
    void reportIssue();
    void suggestFeature();
    void showChangelog();

   private:
    UserExperienceManager() = default;

    // Internal state
    ExperienceLevel userLevel_ = ExperienceLevel::Beginner;
    std::map<std::string, bool> completedActions_;
    std::vector<std::string> achievements_;
    std::map<GuidanceContext, bool> guidancePreferences_;

    // Data initialization
    void initializeTutorials();
    void initializeHelpEntries();
    void initializeCompletionData();
    void loadUserPreferences();
    void saveUserPreferences();

    // Tutorial management
    std::map<std::string, Tutorial> tutorials_;
    std::map<std::string, HelpEntry> helpEntries_;
    std::map<std::string, std::vector<CompletionSuggestion>> completionData_;

    // Experience detection helpers
    bool hasExistingProjects();
    bool hasAdvancedToolsInstalled();
    int estimateExperienceFromHistory();

    // Guidance helpers
    void showStepByStepGuidance(const std::vector<std::string>& steps);
    void showProgressIndicator(int current, int total, const std::string& stepName);
    bool confirmUserAction(const std::string& action);

    // Tutorial execution helpers
    bool executeTutorialStep(const TutorialStep& step);
    void showTutorialProgress(int currentStep, int totalSteps);
    bool validateStepCompletion(const TutorialStep& step);

    // Completion helpers
    std::vector<CompletionSuggestion> filterCompletions(
            const std::vector<CompletionSuggestion>& suggestions, const std::string& input);
    void sortCompletionsByRelevance(std::vector<CompletionSuggestion>& suggestions,
                                    const std::string& input);

    // Help system helpers
    void formatAndShowHelp(const HelpEntry& entry);
    std::vector<std::string> extractKeywords(const std::string& query);
    int calculateHelpRelevance(const HelpEntry& entry, const std::vector<std::string>& keywords);
};

// Utility functions for user experience
namespace ux_utils {
// Experience level utilities
std::string toString(ExperienceLevel level);
ExperienceLevel parseExperienceLevel(const std::string& str);
bool isValidExperienceLevel(const std::string& str);

// Guidance context utilities
std::string toString(GuidanceContext context);
GuidanceContext parseGuidanceContext(const std::string& str);

// Interactive utilities
void showProgressBar(int current, int total, const std::string& label = "");
void showSpinner(const std::string& message, int durationMs = 2000);
void showTypingEffect(const std::string& text, int delayMs = 50);
void showHighlight(const std::string& text);

// Formatting utilities
std::string formatTutorialStep(const TutorialStep& step, int stepNumber);
std::string formatHelpEntry(const HelpEntry& entry);
std::string formatCompletion(const CompletionSuggestion& suggestion);
std::string formatExampleCode(const std::string& code, const std::string& language = "cpp");

// Validation utilities
bool validateTutorial(const Tutorial& tutorial);
bool validateHelpEntry(const HelpEntry& entry);
bool validateCompletionSuggestion(const CompletionSuggestion& suggestion);

// User interaction utilities
bool askYesNo(const std::string& question, bool defaultValue = true);
int askChoice(const std::string& question, const std::vector<std::string>& options,
              int defaultChoice = 0);
std::string askInput(const std::string& prompt, const std::string& defaultValue = "");
std::vector<std::string> askMultipleChoice(const std::string& question,
                                           const std::vector<std::string>& options,
                                           const std::vector<bool>& defaultSelected = {});

// Display utilities
void showBanner(const std::string& title, const std::string& subtitle = "");
void showSection(const std::string& title, const std::vector<std::string>& content);
void showCodeBlock(const std::string& code, const std::string& language = "cpp");
void showTip(const std::string& tip);
void showWarning(const std::string& warning);
void showNote(const std::string& note);

// File and path utilities for examples
std::vector<std::string> findExampleProjects();
std::string getExamplePath(const std::string& exampleName);
bool copyExampleProject(const std::string& exampleName, const std::string& destination);
}  // namespace ux_utils

}  // namespace utils
