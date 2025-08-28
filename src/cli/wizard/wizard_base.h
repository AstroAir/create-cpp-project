#pragma once

#include "../types/cli_options.h"
#include <string>

// Base interface for wizard steps
class WizardStepBase {
public:
  virtual ~WizardStepBase() = default;

  // Configure the options for this step
  virtual bool configure(CliOptions& options) = 0;

  // Get the step name for display
  virtual std::string getStepName() const = 0;

  // Get step description
  virtual std::string getStepDescription() const = 0;

  // Check if this step should be skipped based on current options
  virtual bool shouldSkip(const CliOptions& options) const { (void)options; return false; }

  // Validate the configuration for this step
  virtual bool validate(const CliOptions& options) const { (void)options; return true; }
};

// Wizard progress tracking
struct WizardProgress {
  int currentStep = 0;
  int totalSteps = 0;
  std::string currentStepName;

  void update(int step, int total, const std::string& stepName) {
    currentStep = step;
    totalSteps = total;
    currentStepName = stepName;
  }

  int getPercentage() const {
    if (totalSteps == 0) return 0;
    return (currentStep * 100) / totalSteps;
  }
};

// Wizard utilities
namespace wizard_utils {

// Show wizard progress
void showProgress(const WizardProgress& progress);

// Show wizard header
void showWizardHeader(const std::string& title, int currentStep, int totalSteps);

// Show step completion
void showStepComplete(const std::string& stepName);

// Show step error
void showStepError(const std::string& stepName, const std::string& error);

// Show welcome screens
void showWelcomeScreen();
void showEnhancedWelcomeScreen();
void showNpmStyleWelcomeScreen();

} // namespace wizard_utils
