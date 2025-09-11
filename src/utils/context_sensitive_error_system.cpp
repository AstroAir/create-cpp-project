#include "context_sensitive_error_system.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <iostream>
#include <regex>

#include "terminal_utils.h"

namespace utils {

ContextSensitiveErrorSystem& ContextSensitiveErrorSystem::getInstance() {
    static ContextSensitiveErrorSystem instance;
    return instance;
}

ContextSensitiveErrorSystem::ContextSensitiveErrorSystem() {
    initializeErrorPatterns();
}

void ContextSensitiveErrorSystem::reportError(const std::string& message, ErrorContext context,
                                              const std::string& userAction,
                                              const std::map<std::string, std::string>& metadata) {
    ContextualError error;
    error.message = message;
    error.context = context;
    error.userAction = userAction;
    error.metadata = metadata;
    error.timestamp = std::chrono::system_clock::now();

    reportContextualError(error);
}

void ContextSensitiveErrorSystem::reportContextualError(const ContextualError& error) {
    // Create a mutable copy for enrichment
    ContextualError enrichedError = error;

    // Enrich with context and system state
    enrichErrorWithContext(enrichedError);
    analyzeSystemState(enrichedError);

    // Generate solutions
    enrichedError.solutions = generateSolutions(enrichedError);

    // Store in history
    errorHistory_.push_back(enrichedError);
    errorCounts_[error.context]++;

    // Log the error
    logError(enrichedError);

    // Show error to user
    if (interactiveMode_) {
        showInteractiveErrorDialog(enrichedError);
    } else {
        showDetailedErrorInfo(enrichedError);
    }

    // Offer automatic fix if available
    if (autoFixEnabled_) {
        offerAutomaticFix(enrichedError);
    }
}

void ContextSensitiveErrorSystem::reportProjectCreationError(const std::string& message,
                                                             const std::string& projectPath,
                                                             const std::string& templateType) {
    std::map<std::string, std::string> metadata;
    metadata["project_path"] = projectPath;
    if (!templateType.empty()) {
        metadata["template_type"] = templateType;
    }

    reportError(message, ErrorContext::ProjectCreation, "Creating new project", metadata);
}

void ContextSensitiveErrorSystem::reportFileOperationError(const std::string& operation,
                                                           const std::string& filePath,
                                                           const std::string& systemError) {
    std::map<std::string, std::string> metadata;
    metadata["operation"] = operation;
    metadata["file_path"] = filePath;
    if (!systemError.empty()) {
        metadata["system_error"] = systemError;
    }

    std::string message = "Failed to " + operation + " file: " + filePath;
    if (!systemError.empty()) {
        message += " (" + systemError + ")";
    }

    reportError(message, ErrorContext::FileOperations, "File operation: " + operation, metadata);
}

void ContextSensitiveErrorSystem::reportBuildSystemError(const std::string& buildSystem,
                                                         const std::string& command,
                                                         const std::string& output) {
    std::map<std::string, std::string> metadata;
    metadata["build_system"] = buildSystem;
    metadata["command"] = command;
    if (!output.empty()) {
        metadata["build_output"] = output;
    }

    std::string message = buildSystem + " build failed";
    reportError(message, ErrorContext::BuildSystem, "Building project with " + buildSystem,
                metadata);
}

std::vector<ErrorSolution> ContextSensitiveErrorSystem::generateSolutions(
        const ContextualError& error) {
    std::vector<ErrorSolution> solutions;

    // Try pattern-based solutions first
    for (const auto& pattern : errorPatterns_) {
        if (pattern.context == error.context) {
            std::regex regex(pattern.pattern);
            if (std::regex_search(error.message, regex)) {
                auto patternSolutions = pattern.solutionGenerator(error);
                solutions.insert(solutions.end(), patternSolutions.begin(), patternSolutions.end());
            }
        }
    }

    // Generate context-specific solutions
    switch (error.context) {
        case ErrorContext::ProjectCreation: {
            auto contextSolutions = generateProjectCreationSolutions(error);
            solutions.insert(solutions.end(), contextSolutions.begin(), contextSolutions.end());
        } break;
        case ErrorContext::FileOperations: {
            auto contextSolutions = generateFileOperationSolutions(error);
            solutions.insert(solutions.end(), contextSolutions.begin(), contextSolutions.end());
        } break;
        case ErrorContext::BuildSystem: {
            auto contextSolutions = generateBuildSystemSolutions(error);
            solutions.insert(solutions.end(), contextSolutions.begin(), contextSolutions.end());
        } break;
        case ErrorContext::PackageManagement: {
            auto contextSolutions = generatePackageManagerSolutions(error);
            solutions.insert(solutions.end(), contextSolutions.begin(), contextSolutions.end());
        } break;
        case ErrorContext::GitOperations: {
            auto contextSolutions = generateGitSolutions(error);
            solutions.insert(solutions.end(), contextSolutions.begin(), contextSolutions.end());
        } break;
        case ErrorContext::Configuration: {
            auto contextSolutions = generateConfigurationSolutions(error);
            solutions.insert(solutions.end(), contextSolutions.begin(), contextSolutions.end());
        } break;
        default:
            break;
    }

    // Sort solutions by priority
    std::sort(
            solutions.begin(), solutions.end(),
            [](const ErrorSolution& a, const ErrorSolution& b) { return a.priority > b.priority; });

    return solutions;
}

std::vector<ErrorSolution> ContextSensitiveErrorSystem::generateProjectCreationSolutions(
        const ContextualError& error) {
    std::vector<ErrorSolution> solutions;

    // Check for common project creation issues
    if (error.message.find("permission") != std::string::npos) {
        ErrorSolution solution;
        solution.type = SolutionType::Manual;
        solution.title = "Fix Directory Permissions";
        solution.description = "The target directory may not have write permissions";
        solution.steps = {"Check if you have write access to the target directory",
                          "Try creating the project in a different location",
                          "Run with administrator privileges if necessary"};
        solution.priority = 90;
        solutions.push_back(solution);
    }

    if (error.message.find("already exists") != std::string::npos) {
        ErrorSolution solution;
        solution.type = SolutionType::Interactive;
        solution.title = "Handle Existing Directory";
        solution.description = "A directory with the same name already exists";
        solution.steps = {"Choose a different project name", "Remove the existing directory",
                          "Use the --force flag to overwrite"};
        solution.priority = 95;
        solutions.push_back(solution);
    }

    return solutions;
}

std::vector<ErrorSolution> ContextSensitiveErrorSystem::generateFileOperationSolutions(
        const ContextualError& error) {
    std::vector<ErrorSolution> solutions;

    auto operation = error.metadata.find("operation");
    if (operation != error.metadata.end()) {
        if (operation->second == "write" || operation->second == "create") {
            ErrorSolution solution;
            solution.type = SolutionType::Automatic;
            solution.title = "Create Missing Directories";
            solution.description = "Create parent directories if they don't exist";
            solution.autoFix = [&error]() {
                // Implementation would create missing directories
                return true;
            };
            solution.priority = 85;
            solutions.push_back(solution);
        }

        if (operation->second == "read") {
            ErrorSolution solution;
            solution.type = SolutionType::Manual;
            solution.title = "Check File Existence";
            solution.description = "Verify the file exists and is readable";
            solution.steps = {"Check if the file path is correct", "Verify file permissions",
                              "Ensure the file hasn't been moved or deleted"};
            solution.priority = 80;
            solutions.push_back(solution);
        }
    }

    return solutions;
}

void ContextSensitiveErrorSystem::showInteractiveErrorDialog(const ContextualError& error) {
    // Show error header
    showErrorHeader(error);

    // Show error details
    spdlog::error(error.message);

    if (!error.userAction.empty()) {
        std::cout << "  " << TerminalUtils::colorize("While:", Color::BrightWhite) << " "
                  << error.userAction << "\n";
    }

    // Show solutions if available
    if (!error.solutions.empty()) {
        std::cout << "\n";
        showSolutionsList(error.solutions);

        // Basic confirmation for applying solutions
        std::cout << "Apply automatic fixes? (y/n): ";
        std::string response;
        std::getline(std::cin, response);

        if (response == "y" || response == "Y") {
            // Try to apply the first automatic solution
            for (const auto& solution : error.solutions) {
                if (solution.type == SolutionType::Automatic && solution.autoFix) {
                    if (applySolution(solution, error)) {
                        spdlog::info("Solution applied successfully!");
                        return;
                    }
                }
            }
            spdlog::warn("No automatic solutions available");
        }
    }
}

void ContextSensitiveErrorSystem::showErrorHeader(const ContextualError& error) {
    std::string icon = getContextIcon(error.context);
    std::string contextName;

    switch (error.context) {
        case ErrorContext::ProjectCreation:
            contextName = "Project Creation";
            break;
        case ErrorContext::FileOperations:
            contextName = "File Operations";
            break;
        case ErrorContext::BuildSystem:
            contextName = "Build System";
            break;
        case ErrorContext::PackageManagement:
            contextName = "Package Management";
            break;
        case ErrorContext::GitOperations:
            contextName = "Git Operations";
            break;
        case ErrorContext::Configuration:
            contextName = "Configuration";
            break;
        default:
            contextName = "System";
            break;
    }

    std::cout << "\n"
              << TerminalUtils::colorize("  " + icon + " " + contextName + " Error",
                                         Color::BrightRed)
              << "\n\n";
}

std::string ContextSensitiveErrorSystem::getContextIcon(ErrorContext context) {
    switch (context) {
        case ErrorContext::ProjectCreation:
            return "🏗️";
        case ErrorContext::FileOperations:
            return "📁";
        case ErrorContext::BuildSystem:
            return "🔨";
        case ErrorContext::PackageManagement:
            return "📦";
        case ErrorContext::GitOperations:
            return "🔀";
        case ErrorContext::Configuration:
            return "⚙️";
        default:
            return "❌";
    }
}

void ContextSensitiveErrorSystem::initializeErrorPatterns() {
    // Add common error patterns with solutions
    // This would be expanded with more patterns based on common issues
}

void ContextSensitiveErrorSystem::enrichErrorWithContext(ContextualError& error) {
    // Add system context information
    // This would gather relevant system state based on the error context
}

void ContextSensitiveErrorSystem::analyzeSystemState(ContextualError& error) {
    // Analyze current system state to provide better solutions
    // This would check disk space, permissions, network connectivity, etc.
}

void ContextSensitiveErrorSystem::logError(const ContextualError& error) {
    spdlog::error("Context: {} | Message: {} | Action: {}", static_cast<int>(error.context),
                  error.message, error.userAction);
}

}  // namespace utils
