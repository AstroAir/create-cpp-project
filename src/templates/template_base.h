#pragma once
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "../cli/types/cli_options.h"
#include "../testing/test_framework_manager.h"
#include "../utils/edge_case_handler.h"
#include "../utils/file_utils.h"
#include "../utils/progress_indicator.h"

class TemplateBase {
   public:
    TemplateBase(const CliOptions& options);
    virtual ~TemplateBase();

    // Enhanced project creation with comprehensive error handling
    virtual bool create() = 0;

    // Enhanced creation with progress tracking and rollback support
    bool createWithEnhancedHandling();

   protected:
    // Resource management and cleanup
    struct ResourceManager {
        std::vector<std::filesystem::path> createdDirectories;
        std::vector<std::filesystem::path> createdFiles;
        std::vector<std::string> executedCommands;
        std::chrono::steady_clock::time_point startTime;

        void addCreatedDirectory(const std::filesystem::path& path);
        void addCreatedFile(const std::filesystem::path& path);
        void addExecutedCommand(const std::string& command);
        bool rollbackChanges();
        void cleanup();
    };

    std::unique_ptr<ResourceManager> resourceManager_;
    std::unique_ptr<utils::ScopedProgress> currentProgress_;

    CliOptions options_;

    // Enhanced core template methods with error handling
    virtual bool createProjectStructure() = 0;
    virtual bool createBuildSystem() = 0;
    virtual bool setupPackageManager() = 0;
    virtual bool setupTestFramework() = 0;

    // Enhanced file operations with comprehensive error handling
    bool createDirectoryWithValidation(const std::filesystem::path& path);
    bool createFileWithValidation(const std::filesystem::path& path, const std::string& content);
    bool copyFileWithValidation(const std::filesystem::path& source,
                                const std::filesystem::path& dest);
    bool executeCommandWithValidation(const std::string& command,
                                      const std::filesystem::path& workingDir = "");

    // Input validation helpers
    bool validateProjectConfiguration();
    bool validateSystemRequirements();
    bool validateDiskSpace(size_t requiredBytes = 100 * 1024 * 1024);  // 100MB default

    // Progress tracking helpers
    void startProgressStep(const std::string& stepName, const std::string& description = "");
    void updateProgress(double progress, const std::string& message = "");
    void completeProgressStep(const std::string& message = "");
    void failProgressStep(const std::string& error);

    // Error handling helpers
    void reportTemplateError(const std::string& operation, const std::string& details,
                             const std::string& suggestion = "");
    bool handleFileSystemError(const std::filesystem::path& path, const std::string& operation);
    bool attemptErrorRecovery(const std::string& errorCode);

    // Resource cleanup helpers
    void registerCreatedResource(const std::filesystem::path& path, bool isDirectory = false);
    bool cleanupOnFailure();

    // 设置CI/CD配置
    bool setupCICD(const std::string& projectPath);

    // 设置编辑器配置
    bool setupEditorConfig(const std::string& projectPath);

    // 设置代码风格配置
    bool setupCodeStyleConfig(const std::string& projectPath);

    // 设置文档配置
    bool setupDocConfig(const std::string& projectPath);

    // 初始化git
    bool initializeGit(const std::string& projectPath);

    // 获取.gitignore内容
    std::string getGitignoreContent();

    // 获取许可证内容
    std::string getLicenseContent(const std::string& projectName);

    // 创建许可证文件
    bool createLicense(const std::string& projectPath);

    // 创建空的.clang-format文件
    bool createClangFormat(const std::string& projectPath);

    // 创建.gitattributes文件
    bool createGitAttributes(const std::string& projectPath);

    // 打印项目创建后的使用指南
    void printUsageGuide();

    // 执行创建后的操作
    bool executePostCreationActions();
};
