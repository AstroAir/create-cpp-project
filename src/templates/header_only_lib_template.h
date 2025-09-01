#pragma once
#include "template_base.h"

class HeaderOnlyLibTemplate : public TemplateBase {
   public:
    HeaderOnlyLibTemplate(const CliOptions& options);

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
    // Header-only library specific content generators
    std::string getMainHeaderContent();
    std::string getDetailHeaderContent();
    std::string getVersionHeaderContent();
    std::string getConfigHeaderContent();
    std::string getExampleUsageContent();
    std::string getReadmeContent();
    std::string getCMakeContent();
    std::string getMesonContent();
    std::string getBazelContent();
    std::string getXMakeContent();
    std::string getPremakeContent();
    std::string getVcpkgJsonContent();
    std::string getConanfileContent();
    std::string getGTestContent();
    std::string getCatch2Content();
    std::string getDocTestContent();
    std::string getDoxygenContent();
    std::string getMkDocsContent();
    std::string getGitHubActionsContent();
    std::string getGitLabCIContent();
    std::string getClangFormatContent();
    std::string getClangTidyContent();

    // Documentation content generators
    std::string getDocumentationReadme();
    std::string getAPIDocumentationTemplate();
    std::string getExamplesDocumentationTemplate();
    std::string getDocumentationScript();
    std::string getEditorConfigContent();
    std::string getPkgConfigContent();
    std::string getCMakeConfigContent();
    std::string getSingleHeaderScript();
    std::string getBenchmarkContent();
    std::string getInstallationGuideContent();
    std::string getContributingGuideContent();
    std::string getChangelogContent();
    std::string getLicenseContent();

    // Helper methods
    std::string getProjectNameUpper() const;
    std::string getProjectNamespace() const;
    std::string getIncludeGuard(const std::string& filename) const;
    std::string getCurrentYear() const;
    bool createSingleHeaderVersion();
    bool createExamples();
    bool createBenchmarks();
    bool createDocumentation();
};
