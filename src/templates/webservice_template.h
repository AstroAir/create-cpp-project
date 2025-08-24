#pragma once
#include "template_base.h"

class WebServiceTemplate : public TemplateBase {
public:
  WebServiceTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;
  bool setupDockerConfiguration();
  bool setupAPIDocumentation();
  bool setupLogging();
  bool setupDatabase();

private:
  // Core application files
  std::string getMainCppContent();
  std::string getServerHeaderContent();
  std::string getServerCppContent();
  std::string getRouterHeaderContent();
  std::string getRouterCppContent();
  std::string getControllerHeaderContent();
  std::string getControllerCppContent();
  std::string getMiddlewareHeaderContent();
  std::string getMiddlewareCppContent();
  std::string getConfigHeaderContent();
  std::string getConfigCppContent();
  
  // Database related
  std::string getDatabaseHeaderContent();
  std::string getDatabaseCppContent();
  std::string getModelHeaderContent();
  std::string getModelCppContent();
  
  // Utility files
  std::string getUtilsHeaderContent();
  std::string getUtilsCppContent();
  std::string getLoggerHeaderContent();
  std::string getLoggerCppContent();
  
  // Configuration files
  std::string getReadmeContent();
  std::string getCMakeContent();
  std::string getMesonContent();
  std::string getBazelContent();
  std::string getDockerfileContent();
  std::string getDockerComposeContent();
  std::string getVcpkgJsonContent();
  std::string getConanfileContent();
  
  // API Documentation
  std::string getOpenAPIContent();
  std::string getPostmanCollectionContent();
  
  // Testing
  std::string getGTestContent();
  std::string getCatch2Content();
  std::string getIntegrationTestContent();
  std::string getLoadTestContent();
  
  // Configuration files
  std::string getConfigJsonContent();
  std::string getEnvironmentContent();
  
  // CI/CD
  std::string getGitHubActionsContent();
  std::string getDockerIgnoreContent();
  
  // Helper methods
  std::string getWebFramework() const;
  std::string getDatabaseType() const;
  bool isRestAPI() const;
  bool isGraphQL() const;
};
