#include "project_wizard.h"

#include <spdlog/spdlog.h>

#include <iostream>

#include "../../utils/terminal_utils.h"

using namespace utils;

// Constructor
ProjectWizard::ProjectWizard() {
    initializeSteps();
    setupProgress();
}

// Main wizard method
CliOptions ProjectWizard::runWizard(const CliOptions& initialOptions) {
    CliOptions options = initialOptions;

    try {
        showWelcome();

        if (!executeAllSteps(options)) {
            TerminalUtils::showError("Wizard execution failed");
            return options;
        }

        if (!validateFinalConfiguration(options)) {
            TerminalUtils::showError("Final configuration validation failed");
            return options;
        }

        showCompletion(options);

        // Offer to save configuration
        offerToSaveDefaults(options);
        offerToSaveTemplate(options);

        return options;

    } catch (const std::exception& e) {
        TerminalUtils::showError("Wizard error: " + std::string(e.what()));
        return options;
    }
}

// Quick start wizard
CliOptions ProjectWizard::runQuickStartWizard() {
    CliOptions options;

    TerminalUtils::showInfo("Running Quick Start Wizard...");

    // Set sensible defaults
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::None;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    options.initGit = true;

    // Only ask for project name
    std::cout << "Enter project name: ";
    std::getline(std::cin, options.projectName);

    if (options.projectName.empty()) {
        options.projectName = "my-cpp-project";
    }

    return options;
}

// Advanced wizard
CliOptions ProjectWizard::runAdvancedWizard(const CliOptions& baseOptions) {
    CliOptions options = baseOptions;

    TerminalUtils::showInfo("Running Advanced Wizard...");

    // Run all wizard steps
    return runWizard(options);
}

// Template selection wizard
CliOptions ProjectWizard::runTemplateSelectionWizard() {
    CliOptions options;

    TerminalUtils::showInfo("Template Selection Wizard");

    // Show available templates
    std::cout << "\nAvailable Templates:\n";
    std::cout << "1. Console Application\n";
    std::cout << "2. Library\n";
    std::cout << "3. Header-Only Library\n";
    std::cout << "4. GUI Application\n";
    std::cout << "5. Network Application\n";

    std::cout << "\nSelect template (1-5): ";
    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            options.templateType = TemplateType::Console;
            break;
        case 2:
            options.templateType = TemplateType::Lib;
            break;
        case 3:
            options.templateType = TemplateType::HeaderOnlyLib;
            break;
        case 4:
            options.templateType = TemplateType::Gui;
            break;
        case 5:
            options.templateType = TemplateType::Network;
            break;
        default:
            options.templateType = TemplateType::Console;
            break;
    }

    return getRecommendedOptionsForTemplate(options.templateType);
}

// Show summary and confirm
bool ProjectWizard::showSummaryAndConfirm(const CliOptions& options) {
    TerminalUtils::showInfo("Configuration Summary:");

    std::cout << "Project Name: " << options.projectName << "\n";
    std::cout << "Template Type: " << static_cast<int>(options.templateType) << "\n";
    std::cout << "Build System: " << static_cast<int>(options.buildSystem) << "\n";
    std::cout << "Package Manager: " << static_cast<int>(options.packageManager) << "\n";
    std::cout << "Include Tests: " << (options.includeTests ? "Yes" : "No") << "\n";

    std::cout << "\nProceed with this configuration? (y/n): ";
    char response;
    std::cin >> response;

    return (response == 'y' || response == 'Y');
}

// Save as template
bool ProjectWizard::saveAsTemplate(const CliOptions& /* options */,
                                   const std::string& templateName) {
    TerminalUtils::showInfo("Saving configuration as template: " + templateName);
    // TODO: Implement template saving logic
    return true;
}

// Get recommended options for template
CliOptions ProjectWizard::getRecommendedOptionsForTemplate(TemplateType type) {
    CliOptions options;
    options.templateType = type;

    switch (type) {
        case TemplateType::Console:
            options.buildSystem = BuildSystem::CMake;
            options.packageManager = PackageManager::None;
            options.includeTests = true;
            options.testFramework = TestFramework::GTest;
            break;

        case TemplateType::Lib:
            options.buildSystem = BuildSystem::CMake;
            options.packageManager = PackageManager::Vcpkg;
            options.includeTests = true;
            options.testFramework = TestFramework::GTest;
            break;

        case TemplateType::HeaderOnlyLib:
            options.buildSystem = BuildSystem::CMake;
            options.packageManager = PackageManager::None;
            options.includeTests = true;
            options.testFramework = TestFramework::Catch2;
            break;

        default:
            options.buildSystem = BuildSystem::CMake;
            options.packageManager = PackageManager::None;
            options.includeTests = true;
            options.testFramework = TestFramework::GTest;
            break;
    }

    return options;
}

// Private methods
void ProjectWizard::initializeSteps() {
    // TODO: Initialize wizard steps
    m_steps.clear();
}

void ProjectWizard::setupProgress() {
    m_progress.currentStep = 0;
    m_progress.totalSteps = static_cast<int>(m_steps.size());
    m_progress.currentStepName = "";
}

bool ProjectWizard::executeStep(WizardStepBase& step, CliOptions& options) {
    try {
        showStepProgress(m_progress.currentStep + 1, m_progress.totalSteps, step.getStepName());

        bool success = step.configure(options);

        if (success) {
            m_progress.currentStep++;
            TerminalUtils::showSuccess("Step completed: " + step.getStepName());
        } else {
            handleStepError(step.getStepName(), "Step execution failed");
        }

        return success;

    } catch (const std::exception& e) {
        handleStepError(step.getStepName(), e.what());
        return false;
    }
}

bool ProjectWizard::executeAllSteps(CliOptions& options) {
    for (auto& step : m_steps) {
        if (!executeStep(*step, options)) {
            if (!askToContinueAfterError()) {
                return false;
            }
        }
    }

    // All steps completed
    return true;
}

void ProjectWizard::showWelcome() {
    TerminalUtils::showInfo("Welcome to the C++ Project Wizard!");
    std::cout << "This wizard will guide you through creating a new C++ project.\n\n";
}

void ProjectWizard::showCompletion(const CliOptions& options) {
    TerminalUtils::showSuccess("Project configuration completed!");
    std::cout << "Project '" << options.projectName << "' is ready to be created.\n";
}

void ProjectWizard::showStepProgress(int currentStep, int totalSteps, const std::string& stepName) {
    std::cout << "\n[" << currentStep << "/" << totalSteps << "] " << stepName << "\n";
    std::cout << std::string(50, '-') << "\n";
}

bool ProjectWizard::offerToSaveDefaults(const CliOptions& /* options */) {
    std::cout << "\nSave these settings as default? (y/n): ";
    char response;
    std::cin >> response;

    if (response == 'y' || response == 'Y') {
        TerminalUtils::showInfo("Saving default configuration...");
        // TODO: Implement saving logic
        return true;
    }

    return false;
}

bool ProjectWizard::offerToSaveTemplate(const CliOptions& options) {
    std::cout << "\nSave as custom template? (y/n): ";
    char response;
    std::cin >> response;

    if (response == 'y' || response == 'Y') {
        std::cout << "Enter template name: ";
        std::string templateName;
        std::cin >> templateName;

        return saveAsTemplate(options, templateName);
    }

    return false;
}

bool ProjectWizard::validateFinalConfiguration(const CliOptions& options) {
    if (options.projectName.empty()) {
        TerminalUtils::showError("Project name cannot be empty");
        return false;
    }

    return true;
}

void ProjectWizard::handleStepError(const std::string& stepName, const std::string& error) {
    TerminalUtils::showError("Error in step '" + stepName + "': " + error);
}

bool ProjectWizard::askToContinueAfterError() {
    std::cout << "Continue with next step? (y/n): ";
    char response;
    std::cin >> response;

    return (response == 'y' || response == 'Y');
}
