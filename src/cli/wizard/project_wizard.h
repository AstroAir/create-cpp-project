#pragma once

#include "wizard_base.h"
#include "project_details_wizard.h"
#include "build_system_wizard.h"
#include "testing_wizard.h"
#include "dev_tools_wizard.h"
#include "cicd_wizard.h"
#include "../types/cli_options.h"
#include <memory>
#include <vector>

// Main project wizard that orchestrates all configuration steps
class ProjectWizard {
public:
  // Constructor
  ProjectWizard();

  // Main wizard methods
  CliOptions runWizard(const CliOptions& initialOptions = {});
  CliOptions runQuickStartWizard();
  CliOptions runAdvancedWizard(const CliOptions& baseOptions);
  CliOptions runTemplateSelectionWizard();

  // Configuration summary and confirmation
  bool showSummaryAndConfirm(const CliOptions& options);

  // Template and profile management
  bool saveAsTemplate(const CliOptions& options, const std::string& templateName);
  CliOptions getRecommendedOptionsForTemplate(TemplateType type);

private:
  // Wizard steps
  std::vector<std::unique_ptr<WizardStepBase>> m_steps;
  WizardProgress m_progress;

  // Setup methods
  void initializeSteps();
  void setupProgress();

  // Execution methods
  bool executeStep(WizardStepBase& step, CliOptions& options);
  bool executeAllSteps(CliOptions& options);

  // Display methods
  void showWelcome();
  void showCompletion(const CliOptions& options);
  void showStepProgress(int currentStep, int totalSteps, const std::string& stepName);

  // Configuration management
  bool offerToSaveDefaults(const CliOptions& options);
  bool offerToSaveTemplate(const CliOptions& options);

  // Validation
  bool validateFinalConfiguration(const CliOptions& options);

  // Error handling
  void handleStepError(const std::string& stepName, const std::string& error);
  bool askToContinueAfterError();
};
