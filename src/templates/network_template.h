#pragma once
#include "template_base.h"

class NetworkTemplate : public TemplateBase {
public:
  NetworkTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;
  bool setupCodeStyleTools();
  bool setupEditorIntegrations();
  bool setupCICDIntegrations();

private:
  bool setupNetworkLibrary();
  std::string getMainCppContent();
  std::string getServerHeaderContent();
  std::string getServerImplContent();
  std::string getClientHeaderContent();
  std::string getClientImplContent();
  std::string getReadmeContent();
  std::string getCMakeContent();
  std::string getMesonContent();
  std::string getBazelContent();
  std::string getXMakeContent();
  std::string getPremakeContent();
  std::string getNetworkTestContent();
};
