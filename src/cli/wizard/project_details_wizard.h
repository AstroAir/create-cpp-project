#pragma once

#include "wizard_base.h"
#include "../types/cli_options.h"

// Wizard step for configuring project details
class ProjectDetailsWizard : public WizardStepBase {
public:
  bool configure(CliOptions& options) override;
  std::string getStepName() const override { return "Project Details"; }
  std::string getStepDescription() const override {
    return "Configure project name, type, and basic settings";
  }

private:
  // Helper methods
  bool configureProjectName(CliOptions& options);
  bool configureProjectPath(CliOptions& options);
  bool configureProjectType(CliOptions& options);
  bool configureNetworkLibrary(CliOptions& options);

  // Validation helpers
  bool validateProjectName(const std::string& name);
  bool validateProjectPath(const std::string& path);

  // Display helpers
  void showTemplateOptions();
  void showTemplateDescription(TemplateType type);
};
