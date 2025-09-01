#include "wizard_base.h"

#include <iostream>

#include "../../utils/terminal_utils.h"

using namespace utils;

// WizardStepBase implementation
bool WizardStepBase::configure(CliOptions& /* options */) {
    // Default implementation - should be overridden by derived classes
    TerminalUtils::showInfo("Configuring step: " + getStepName());
    return true;
}

std::string WizardStepBase::getStepName() const {
    return "Base Step";
}

std::string WizardStepBase::getStepDescription() const {
    return "Base wizard step";
}

// Wizard utilities namespace implementation
namespace wizard_utils {

void showProgress(const WizardProgress& progress) {
    int percentage = progress.getPercentage();
    int barWidth = 40;
    int filledWidth = (percentage * barWidth) / 100;

    std::cout << "\nProgress: [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else if (i == filledWidth) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << percentage << "% (" << progress.currentStep << "/" << progress.totalSteps
              << ")\n";
}

void showWizardHeader(const std::string& title, int currentStep, int totalSteps) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << " [Step " << currentStep << " of " << totalSteps << "]\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void showStepComplete(const std::string& stepName) {
    TerminalUtils::showSuccess("✓ " + stepName + " completed");
}

void showStepError(const std::string& stepName, const std::string& error) {
    TerminalUtils::showError("✗ " + stepName + " failed: " + error);
}

void showWelcomeScreen() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║                 C++ Project Wizard                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║         Create modern C++ projects with ease!               ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void showEnhancedWelcomeScreen() {
    std::cout << "\n";
    std::cout << "🚀 Welcome to the Enhanced C++ Project Wizard!\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "\n";
    std::cout << "This wizard will help you create a modern C++ project with:\n";
    std::cout << "  ✓ Modern build systems (CMake, Meson, etc.)\n";
    std::cout << "  ✓ Package management (vcpkg, Conan, etc.)\n";
    std::cout << "  ✓ Testing frameworks (GTest, Catch2, etc.)\n";
    std::cout << "  ✓ CI/CD configurations\n";
    std::cout << "  ✓ Best practices and project structure\n";
    std::cout << "\n";
}

void showNpmStyleWelcomeScreen() {
    std::cout << "\n";
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                                                             │\n";
    std::cout << "│   🎯 C++ Project Scaffolding Tool                          │\n";
    std::cout << "│                                                             │\n";
    std::cout << "│   Create production-ready C++ projects in seconds!         │\n";
    std::cout << "│                                                             │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    std::cout << "Features:\n";
    std::cout << "  📦 Multiple project templates\n";
    std::cout << "  🔧 Configurable build systems\n";
    std::cout << "  🧪 Integrated testing setup\n";
    std::cout << "  📚 Documentation generation\n";
    std::cout << "  🔄 CI/CD pipeline templates\n";
    std::cout << "\n";
}

}  // namespace wizard_utils
