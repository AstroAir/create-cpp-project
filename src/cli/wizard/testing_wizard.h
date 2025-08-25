#pragma once

#include "wizard_base.h"
#include "../types/cli_options.h"

// Wizard step for configuring testing framework
class TestingWizard : public WizardStepBase {
public:
  bool configure(CliOptions& options) override;
  std::string getStepName() const override { return "Testing Framework"; }
  std::string getStepDescription() const override {
    return "Configure unit testing and test framework";
  }

  bool shouldSkip(const CliOptions& options) const override {
    // Skip if tests are explicitly disabled
    return !options.includeTests;
  }

private:
  // Helper methods
  bool configureIncludeTests(CliOptions& options);
  bool configureTestFramework(CliOptions& options);
  bool configureTestStructure(CliOptions& options);

  // Display helpers
  void showTestingBenefits();
  void showTestFrameworkComparison();
  void showTestStructureOptions();

  // Recommendation helpers
  TestFramework getRecommendedTestFramework(TemplateType templateType);
};
