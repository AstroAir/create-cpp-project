#pragma once

#include "wizard_base.h"
#include "../types/cli_options.h"

// Wizard step for configuring build system and package management
class BuildSystemWizard : public WizardStepBase {
public:
  bool configure(CliOptions& options) override;
  std::string getStepName() const override { return "Build System & Package Manager"; }
  std::string getStepDescription() const override { 
    return "Configure build system and dependency management"; 
  }
  
private:
  // Helper methods
  bool configureBuildSystem(CliOptions& options);
  bool configurePackageManager(CliOptions& options);
  bool configureCppStandard(CliOptions& options);
  bool configureCompilerFlags(CliOptions& options);
  
  // Display helpers
  void showBuildSystemComparison();
  void showPackageManagerInfo();
  void showCppStandardInfo();
  
  // Recommendation helpers
  BuildSystem getRecommendedBuildSystem(TemplateType templateType);
  PackageManager getRecommendedPackageManager(BuildSystem buildSystem);
};
