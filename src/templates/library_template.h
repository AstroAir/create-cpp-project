#pragma once
#include "template_base.h"

class LibraryTemplate : public TemplateBase {
   public:
    LibraryTemplate(const CliOptions& options);

    bool create() override;

   protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;
    bool setupDocumentation();
    bool setupContinuousIntegration();
    bool setupCodeFormatting();
    bool setupBenchmarking();
    bool setupVersionControl();

   private:
    std::string getLibraryCppContent();
    std::string getLibraryHeaderContent();
    std::string getReadmeContent();
    std::string getCMakeContent();
    std::string getMesonContent();
    std::string getBazelContent();
    std::string getXMakeContent();
    std::string getPremakeContent();
    std::string getVcpkgJsonContent();
    std::string getConanfileContent();
    std::string getMSYS2PKGBUILDContent();
    std::string getExampleContent();
    std::string getGTestContent();
    std::string getCatch2Content();
    std::string getDocTestContent();
    std::string getGitignoreContent();
    std::string getGitHubWorkflowContent(const std::string& buildSystem);
    std::string getTravisCIContent(const std::string& buildSystem);
    std::string getAppVeyorContent(const std::string& buildSystem);
    std::string getClangFormatContent();
    std::string getClangTidyContent();
    std::string getDoxyfileContent();
    std::string getSphinxConfigContent();
    std::string getMkDocsContent();
    std::string getVersionHeaderContent();
    std::string getBenchmarkContent();
    std::string getInstallScriptContent();
    std::string getLicenseContent(const std::string& licenseType);
};
