#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <map>

namespace utils {

// Git repository management utilities
class GitUtils {
public:
    // Repository initialization
    static bool initializeRepository(const std::filesystem::path& projectPath);
    static bool isGitRepository(const std::filesystem::path& path);
    static bool hasGitInstalled();

    // .gitignore management
    static bool createGitignore(const std::filesystem::path& projectPath,
                               const std::string& templateType,
                               const std::string& buildSystem = "",
                               const std::string& packageManager = "");

    // Git configuration
    static bool configureRepository(const std::filesystem::path& projectPath,
                                  const std::string& userName = "",
                                  const std::string& userEmail = "");

    // Initial commit
    static bool createInitialCommit(const std::filesystem::path& projectPath,
                                   const std::string& message = "Initial commit");

    // Branch management
    static bool createBranch(const std::filesystem::path& projectPath, const std::string& branchName);
    static bool switchBranch(const std::filesystem::path& projectPath, const std::string& branchName);
    static std::vector<std::string> listBranches(const std::filesystem::path& projectPath);

    // Remote management
    static bool addRemote(const std::filesystem::path& projectPath,
                         const std::string& remoteName,
                         const std::string& remoteUrl);
    static std::vector<std::pair<std::string, std::string>> listRemotes(const std::filesystem::path& projectPath);

    // Git hooks
    static bool installGitHooks(const std::filesystem::path& projectPath,
                               const std::vector<std::string>& hookTypes);

    // Git attributes
    static bool createGitAttributes(const std::filesystem::path& projectPath);

    // Enhanced Git workflow support
    static bool setupGitWorkflow(const std::filesystem::path& projectPath,
                                const std::string& workflowType);
    static bool setupGitFlow(const std::filesystem::path& projectPath);
    static bool setupGitHubFlow(const std::filesystem::path& projectPath);
    static bool setupGitLabFlow(const std::filesystem::path& projectPath);

    // Branch management enhancements
    static bool createBranchesFromStrategy(const std::filesystem::path& projectPath,
                                         const std::string& strategy,
                                         const std::vector<std::string>& additionalBranches = {});

    // License management
    static bool createLicenseFile(const std::filesystem::path& projectPath,
                                const std::string& licenseType,
                                const std::string& projectName = "",
                                const std::string& author = "",
                                const std::string& year = "");

    // Enhanced repository configuration
    static bool configureRepositoryAdvanced(const std::filesystem::path& projectPath,
                                           const std::string& userName = "",
                                           const std::string& userEmail = "",
                                           const std::string& remoteUrl = "",
                                           bool setupHooks = false);

    // Git hooks management
    static bool createPreCommitHook(const std::filesystem::path& projectPath);
    static bool createPrePushHook(const std::filesystem::path& projectPath);
    static bool createCommitMsgHook(const std::filesystem::path& projectPath);

private:
    // Execute git commands
    static bool executeGitCommand(const std::filesystem::path& workingDir,
                                 const std::vector<std::string>& args,
                                 std::string* output = nullptr);

    // .gitignore template generators
    static std::string getCppGitignoreTemplate();
    static std::string getCMakeGitignoreTemplate();
    static std::string getMesonGitignoreTemplate();
    static std::string getBazelGitignoreTemplate();
    static std::string getXMakeGitignoreTemplate();
    static std::string getPremakeGitignoreTemplate();
    static std::string getNinjaGitignoreTemplate();
    static std::string getVcpkgGitignoreTemplate();
    static std::string getConanGitignoreTemplate();
    static std::string getSpackGitignoreTemplate();
    static std::string getHunterGitignoreTemplate();
    static std::string getVSCodeGitignoreTemplate();
    static std::string getVisualStudioGitignoreTemplate();
    static std::string getClionGitignoreTemplate();
    static std::string getVimGitignoreTemplate();
    static std::string getEmacsGitignoreTemplate();
    static std::string getQtGitignoreTemplate();
    static std::string getGameDevGitignoreTemplate();
    static std::string getWebServiceGitignoreTemplate();
    static std::string getEmbeddedGitignoreTemplate();
    static std::string getLibraryGitignoreTemplate();
    static std::string getDocumentationGitignoreTemplate();
    static std::string getTestingGitignoreTemplate();
    static std::string getCICDGitignoreTemplate();
    static std::string getOSGitignoreTemplate();

    // Git hook templates
    static std::string getPreCommitHookTemplate();
    static std::string getPrePushHookTemplate();
    static std::string getCommitMsgHookTemplate();

    // Git attributes template
    static std::string getGitAttributesTemplate();

    // License templates
    static std::string getMITLicenseTemplate(const std::string& author, const std::string& year);
    static std::string getApache2LicenseTemplate(const std::string& author, const std::string& year);
    static std::string getGPL3LicenseTemplate(const std::string& author, const std::string& year);
    static std::string getBSD3LicenseTemplate(const std::string& author, const std::string& year);
    static std::string getBSD2LicenseTemplate(const std::string& author, const std::string& year);
    static std::string getUnlicenseTemplate();

    // Git workflow configuration templates
    static std::string getGitFlowConfigTemplate();
    static std::string getGitHubFlowConfigTemplate();
    static std::string getGitLabFlowConfigTemplate();
};

// Code quality tools integration
class CodeQualityTools {
public:
    // clang-format integration
    static bool createClangFormatConfig(const std::filesystem::path& projectPath,
                                       const std::string& style = "Google");
    static bool formatCode(const std::filesystem::path& projectPath,
                          const std::vector<std::string>& files = {});
    static bool checkFormatting(const std::filesystem::path& projectPath,
                               const std::vector<std::string>& files = {});

    // clang-tidy integration
    static bool createClangTidyConfig(const std::filesystem::path& projectPath,
                                     const std::vector<std::string>& checks = {});
    static bool runClangTidy(const std::filesystem::path& projectPath,
                            const std::vector<std::string>& files = {});

    // cppcheck integration
    static bool runCppcheck(const std::filesystem::path& projectPath,
                           const std::vector<std::string>& files = {});

    // Static analysis tools
    static bool createStaticAnalysisConfig(const std::filesystem::path& projectPath);
    static bool runStaticAnalysis(const std::filesystem::path& projectPath);

    // EditorConfig
    static bool createEditorConfig(const std::filesystem::path& projectPath);

    // Pre-commit hooks for code quality
    static bool setupPreCommitHooks(const std::filesystem::path& projectPath);

private:
    // Configuration templates
    static std::string getClangFormatTemplate(const std::string& style);
    static std::string getClangTidyTemplate(const std::vector<std::string>& checks);
    static std::string getEditorConfigTemplate();
    static std::string getCppcheckConfigTemplate();
    static std::string getPreCommitConfigTemplate();

    // Tool detection
    static bool hasClangFormat();
    static bool hasClangTidy();
    static bool hasCppcheck();
    static std::optional<std::string> findTool(const std::string& toolName);
};

// Git workflow helpers
class GitWorkflow {
public:
    // Common workflows
    static bool setupGitFlow(const std::filesystem::path& projectPath);
    static bool setupGitHubFlow(const std::filesystem::path& projectPath);

    // Issue templates
    static bool createIssueTemplates(const std::filesystem::path& projectPath);
    static bool createPullRequestTemplate(const std::filesystem::path& projectPath);

    // GitHub Actions / GitLab CI integration
    static bool createGitHubWorkflows(const std::filesystem::path& projectPath,
                                     const std::vector<std::string>& workflows);
    static bool createGitLabCI(const std::filesystem::path& projectPath);

    // Conventional commits
    static bool setupConventionalCommits(const std::filesystem::path& projectPath);

private:
    // Workflow templates
    static std::string getGitFlowConfig();
    static std::string getIssueTemplate(const std::string& type);
    static std::string getPullRequestTemplate();
    static std::string getGitHubWorkflowTemplate(const std::string& workflowType);
    static std::string getGitLabCITemplate();
    static std::string getConventionalCommitsConfig();
};

} // namespace utils
