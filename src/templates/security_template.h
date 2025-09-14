#pragma once
#include "template_base.h"
#include "../utils/security/security_config.h"

class SecurityTemplate : public TemplateBase {
public:
  SecurityTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;

private:
  // Security-specific content generators
  std::string getSecureMainCppContent();
  std::string getSecurityHeaderContent();
  std::string getSecurityImplementationContent();
  std::string getReadmeContent();
  
  // Build system configurations with security
  std::string getSecureCMakeContent();
  std::string getSecureMesonContent();
  std::string getSecureBazelContent();
  
  // Static analyzer configurations
  std::string getClangTidyConfig();
  std::string getCppCheckConfig();
  std::string getPVSStudioConfig();
  
  // CI/CD configurations with security
  std::string getGitHubActionsSecurityWorkflow();
  std::string getGitLabCISecurityConfig();
  
  // Security documentation
  std::string getSecurityDocumentation();
  std::string getSecurityChecklist();
  std::string getThreatModelTemplate();
  
  // Helper methods
  bool createSecurityConfigurations();
  bool createStaticAnalysisConfigs();
  bool createSanitizerConfigs();
  bool createSecurityDocumentation();
  bool createSecurityTests();
  
  utils::SecurityConfigManager securityManager_;
  utils::SecurityConfig securityConfig_;
};
