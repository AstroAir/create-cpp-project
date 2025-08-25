#pragma once

#include "template_base.h"
#include "../utils/git_utils.h"
#include "../utils/progress_indicator.h"

class GitRepositoryTemplate : public TemplateBase {
public:
    GitRepositoryTemplate(const CliOptions& options);
    virtual ~GitRepositoryTemplate() = default;

    // Main creation method
    bool create() override;

protected:
    // Template base implementations
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    // Git repository specific methods
    bool cloneRepository();
    bool processClonedRepository();
    bool validateRepositoryStructure();
    bool cleanupRepository();
    bool setupProjectFromRepository();
    
    // Helper methods
    std::string getRepositoryUrl() const;
    std::string getTargetBranch() const;
    std::string getTargetTag() const;
    std::string getTargetCommit() const;
    std::filesystem::path getTempClonePath() const;
    std::filesystem::path getFinalProjectPath() const;
    
    // Authentication helpers
    bool hasAuthentication() const;
    bool setupAuthentication();
    
    // Repository validation
    bool isCppProject(const std::filesystem::path& repoPath) const;
    bool hasValidStructure(const std::filesystem::path& repoPath) const;
    
    // Project adaptation
    bool adaptProjectName(const std::filesystem::path& repoPath);
    bool updateProjectConfiguration(const std::filesystem::path& repoPath);
    bool mergeWithTemplateOptions(const std::filesystem::path& repoPath);

    // Member variables
    std::filesystem::path tempClonePath_;
    std::filesystem::path finalProjectPath_;
    bool repositoryCloned_;
    bool authenticationSetup_;
};
