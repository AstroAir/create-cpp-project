#pragma once
#include "template_base.h"

class ModulesTemplate : public TemplateBase {
public:
  ModulesTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;

private:
  // Module-specific content generators
  std::string getModuleInterfaceContent();
  std::string getModuleImplementationContent();
  std::string getMainCppContent();
  std::string getReadmeContent();

  // Build system configurations for modules
  std::string getCMakeModulesContent();
  std::string getMesonModulesContent();
  std::string getBazelModulesContent();

  // Package manager configurations
  std::string getVcpkgJsonContent();
  std::string getConanfileContent();

  // Test framework configurations for modules
  std::string getGTestModulesContent();
  std::string getCatch2ModulesContent();
  std::string getDocTestModulesContent();

  // Helper methods
  std::string getModuleName() const;
  std::string getCppStandardFlag() const;
  bool createModuleFiles();
  bool createTestModules();
  std::string getBuildInstructions();
  std::string getTestInstructions();
  std::string getTestRunCommand();
};
