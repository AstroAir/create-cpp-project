#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>

namespace utils {

// Context types for error reporting
enum class ErrorContext {
    ProjectCreation,
    FileOperations,
    BuildSystem,
    PackageManagement,
    GitOperations,
    Configuration,
    TemplateProcessing,
    UserInput,
    SystemValidation,
    NetworkOperations
};

// Solution types
enum class SolutionType {
    Automatic,      // Can be applied automatically
    Manual,         // Requires user action
    Interactive,    // Requires user interaction
    External        // Requires external tools/actions
};

// Error solution
struct ErrorSolution {
    SolutionType type;
    std::string title;
    std::string description;
    std::vector<std::string> steps;
    std::function<bool()> autoFix;
    std::string command;        // CLI command to run
    std::string documentation;  // Link to docs
    int priority = 0;          // Higher = more likely to work
};

// Context-sensitive error information
struct ContextualError {
    std::string code;
    std::string message;
    ErrorContext context;
    std::string userAction;     // What the user was trying to do
    std::string systemState;    // Current system state
    std::map<std::string, std::string> metadata;
    std::vector<ErrorSolution> solutions;
    std::chrono::system_clock::time_point timestamp;
    
    // Context-specific information
    std::string projectPath;
    std::string configFile;
    std::string buildSystem;
    std::string packageManager;
    std::vector<std::string> affectedFiles;
};

// Error pattern for matching and solution generation
struct ErrorPattern {
    std::string pattern;        // Regex pattern to match error
    ErrorContext context;
    std::function<std::vector<ErrorSolution>(const ContextualError&)> solutionGenerator;
    int priority = 0;
};

// Context-sensitive error system
class ContextSensitiveErrorSystem {
public:
    static ContextSensitiveErrorSystem& getInstance();
    
    // Main error reporting
    void reportError(const std::string& message, 
                    ErrorContext context,
                    const std::string& userAction = "",
                    const std::map<std::string, std::string>& metadata = {});
    
    void reportContextualError(const ContextualError& error);
    
    // Context-specific error reporting
    void reportProjectCreationError(const std::string& message, 
                                   const std::string& projectPath,
                                   const std::string& templateType = "");
    
    void reportFileOperationError(const std::string& operation,
                                 const std::string& filePath,
                                 const std::string& systemError = "");
    
    void reportBuildSystemError(const std::string& buildSystem,
                               const std::string& command,
                               const std::string& output = "");
    
    void reportPackageManagerError(const std::string& packageManager,
                                  const std::string& package,
                                  const std::string& operation = "install");
    
    void reportGitError(const std::string& gitCommand,
                       const std::string& repository = "",
                       const std::string& gitOutput = "");
    
    void reportConfigurationError(const std::string& configFile,
                                 const std::string& setting = "",
                                 const std::string& value = "");
    
    // Solution management
    void addErrorPattern(const ErrorPattern& pattern);
    std::vector<ErrorSolution> generateSolutions(const ContextualError& error);
    bool applySolution(const ErrorSolution& solution, const ContextualError& error);
    
    // Interactive error handling
    void showInteractiveErrorDialog(const ContextualError& error);
    bool offerAutomaticFix(const ContextualError& error);
    void showDetailedErrorInfo(const ContextualError& error);
    
    // Error history and analytics
    std::vector<ContextualError> getRecentErrors(size_t count = 10);
    std::vector<ContextualError> getErrorsByContext(ErrorContext context);
    void showErrorSummary();
    void exportErrorReport(const std::string& filePath);
    
    // Configuration
    void setAutoFixEnabled(bool enabled) { autoFixEnabled_ = enabled; }
    void setInteractiveMode(bool enabled) { interactiveMode_ = enabled; }
    void setVerboseErrors(bool enabled) { verboseErrors_ = enabled; }

private:
    ContextSensitiveErrorSystem();
    
    // Solution generators for different contexts
    std::vector<ErrorSolution> generateProjectCreationSolutions(const ContextualError& error);
    std::vector<ErrorSolution> generateFileOperationSolutions(const ContextualError& error);
    std::vector<ErrorSolution> generateBuildSystemSolutions(const ContextualError& error);
    std::vector<ErrorSolution> generatePackageManagerSolutions(const ContextualError& error);
    std::vector<ErrorSolution> generateGitSolutions(const ContextualError& error);
    std::vector<ErrorSolution> generateConfigurationSolutions(const ContextualError& error);
    
    // Helper methods
    void initializeErrorPatterns();
    void analyzeSystemState(ContextualError& error);
    void enrichErrorWithContext(ContextualError& error);
    std::string formatErrorForDisplay(const ContextualError& error);
    void logError(const ContextualError& error);
    
    // Visual helpers
    void showErrorHeader(const ContextualError& error);
    void showSolutionsList(const std::vector<ErrorSolution>& solutions);
    void showSolutionDetails(const ErrorSolution& solution);
    std::string getContextIcon(ErrorContext context);
    std::string getContextColor(ErrorContext context);
    
    // Configuration
    bool autoFixEnabled_ = true;
    bool interactiveMode_ = true;
    bool verboseErrors_ = false;
    
    // Error storage
    std::vector<ContextualError> errorHistory_;
    std::vector<ErrorPattern> errorPatterns_;
    
    // Statistics
    std::map<ErrorContext, size_t> errorCounts_;
    std::map<std::string, size_t> solutionSuccessRates_;
};

// Convenience macros for context-specific error reporting
#define REPORT_PROJECT_ERROR(msg, path) \
    ContextSensitiveErrorSystem::getInstance().reportProjectCreationError(msg, path)

#define REPORT_FILE_ERROR(op, path, err) \
    ContextSensitiveErrorSystem::getInstance().reportFileOperationError(op, path, err)

#define REPORT_BUILD_ERROR(system, cmd, output) \
    ContextSensitiveErrorSystem::getInstance().reportBuildSystemError(system, cmd, output)

#define REPORT_GIT_ERROR(cmd, repo, output) \
    ContextSensitiveErrorSystem::getInstance().reportGitError(cmd, repo, output)

} // namespace utils
