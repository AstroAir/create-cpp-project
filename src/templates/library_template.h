#pragma once
#include "template_base.h"

class LibraryTemplate : public TemplateBase {
public:
  LibraryTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;

private:
  std::string getLibraryCppContent();
  std::string getLibraryHeaderContent();
  std::string getReadmeContent();
  std::string getCMakeContent();
  std::string getMesonContent();
  std::string getBazelContent();
  std::string getVcpkgJsonContent();
  std::string getConanfileContent();
  std::string getExampleContent();
  std::string getGTestContent();
  std::string getCatch2Content();
  std::string getDocTestContent();
};