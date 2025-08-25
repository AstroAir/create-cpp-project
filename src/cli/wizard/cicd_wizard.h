#pragma once

#include "wizard_base.h"
#include "../types/cli_options.h"

// Wizard step for configuring CI/CD and Git
class CiCdWizard : public WizardStepBase {
public:
  bool configure(CliOptions& options) override;
  std::string getStepName() const override { return "CI/CD & Git Configuration"; }
  std::string getStepDescription() const override {
    return "Configure version control, CI/CD pipelines, and Git workflow";
  }

private:
  // Helper methods
  bool configureGitInit(CliOptions& options);
  bool configureGitWorkflow(CliOptions& options);
  bool configureGitSettings(CliOptions& options);
  bool configureCiCdSystems(CliOptions& options);
  bool configureLicense(CliOptions& options);

  // Display helpers
  void showGitBenefits();
  void showWorkflowOptions();
  void showCiCdComparison();
  void showLicenseOptions();

  // Recommendation helpers
  std::vector<CiSystem> getRecommendedCiSystems();
  GitWorkflow getRecommendedWorkflow(TemplateType templateType);
  LicenseType getRecommendedLicense(TemplateType templateType);
};
