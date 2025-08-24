#pragma once
#include "template_base.h"

class MultiExecutableTemplate : public TemplateBase {
public:
  MultiExecutableTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;
  bool setupSharedLibrary();
  bool setupExecutables();

private:
  // Content generators for different executables
  std::string getMainExecutableContent();
  std::string getClientExecutableContent();
  std::string getServerExecutableContent();
  std::string getToolExecutableContent();
  
  // Shared library content
  std::string getSharedLibraryHeaderContent();
  std::string getSharedLibrarySourceContent();
  std::string getUtilsHeaderContent();
  std::string getUtilsSourceContent();
  
  // Build system content
  std::string getCMakeContent();
  std::string getMesonContent();
  std::string getBazelContent();
  
  // Package manager content
  std::string getVcpkgJsonContent();
  std::string getConanfileContent();
  
  // Test content
  std::string getGTestContent();
  std::string getCatch2Content();
  std::string getDocTestContent();
  
  // Documentation and configuration
  std::string getReadmeContent();
  std::string getConfigHeaderContent();
  std::string getVersionHeaderContent();
  
  // Helper methods
  std::string getProjectNameUpper() const;
  std::string getIncludeGuard(const std::string& filename) const;
  
  // Executable configurations
  struct ExecutableConfig {
    std::string name;
    std::string description;
    std::string sourceFile;
    bool needsSharedLib;
    std::vector<std::string> additionalSources;
  };
  
  std::vector<ExecutableConfig> getExecutableConfigs() const;
};
