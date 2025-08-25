#pragma once

#include "wizard_base.h"
#include "../types/cli_options.h"

// Wizard step for configuring development tools
class DevToolsWizard : public WizardStepBase {
public:
  bool configure(CliOptions& options) override;
  std::string getStepName() const override { return "Development Tools"; }
  std::string getStepDescription() const override {
    return "Configure editors, code style tools, and development environment";
  }

private:
  // Helper methods
  bool configureCodeStyleTools(CliOptions& options);
  bool configureEditorSupport(CliOptions& options);
  bool configureDocumentation(CliOptions& options);
  bool configureDocumentationOptions(CliOptions& options);

  // Display helpers
  void showCodeStyleBenefits();
  void showEditorOptions();
  void showDocumentationOptions();

  // Recommendation helpers
  std::vector<EditorConfig> getRecommendedEditors();
  bool shouldIncludeCodeStyle(TemplateType templateType);
};
