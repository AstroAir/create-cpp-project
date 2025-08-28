#pragma once
#include "template_base.h"

class ConsoleTemplate : public TemplateBase {
public:
  ConsoleTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;

private:
  std::string getMainCppContent();
  std::string getReadmeContent();
  std::string getCMakeContent();
  std::string getMesonContent();
  std::string getBazelContent();
  std::string getXMakeContent();
  std::string getPremakeContent();
  std::string getVcpkgJsonContent();
  std::string getConanfileContent();
  std::string getMSYS2PKGBUILDContent();
  std::string getGTestContent();
  std::string getCatch2Content();
  std::string getDocTestContent();
};
