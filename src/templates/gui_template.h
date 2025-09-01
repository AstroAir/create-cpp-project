#pragma once
#include "template_base.h"

class GuiTemplate : public TemplateBase {
   public:
    GuiTemplate(const CliOptions& options);

    bool create() override;

   protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;
    bool setupDocumentation();

   private:
    // 确定GUI框架
    std::string guiFramework_;

    // 创建GUI特定文件
    bool createGuiSpecificFiles();

    // 生成各种内容
    std::string getMainCppContent();
    std::string getMainWindowHeaderContent();
    std::string getMainWindowCppContent();
    std::string getAppHeaderContent();
    std::string getAppCppContent();
    std::string getResourcesContent();
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
    std::string getQtUiContent();
    std::string getQtResourceContent();
    std::string getWxResourceContent();
    std::string getGtkGladeContent();
    std::string getLoggingHeaderContent();
    std::string getLoggingCppContent();

    // Framework-specific content generators
    std::string getFrameworkSpecificHeaderContent();
    std::string getFrameworkSpecificSourceContent();
    std::string getGTKSpecificContent();
    std::string getFLTKSpecificContent();
    std::string getImGuiSpecificContent();
};
