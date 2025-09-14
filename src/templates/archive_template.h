#pragma once

#include "template_base.h"
#include "../utils/archive/archive_utils.h"
#include "../utils/external/http_client.h"
#include "../utils/ui/progress_indicator.h"

class ArchiveTemplate : public TemplateBase {
public:
    ArchiveTemplate(const CliOptions& options);
    virtual ~ArchiveTemplate() = default;

    // Main creation method
    bool create() override;

protected:
    // Template base implementations
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    // Archive specific methods
    bool downloadArchive();
    bool extractArchive();
    bool processExtractedContent();
    bool validateArchiveStructure();
    bool cleanupTemporaryFiles();
    bool setupProjectFromArchive();
    
    // Helper methods
    std::string getArchiveSource() const;
    std::filesystem::path getArchivePath() const;
    std::filesystem::path getTempExtractPath() const;
    std::filesystem::path getFinalProjectPath() const;
    bool isRemoteArchive() const;
    bool isLocalArchive() const;
    
    // Download helpers
    bool downloadRemoteArchive();
    bool validateLocalArchive();
    
    // Extraction helpers
    bool extractToTemporaryLocation();
    bool findProjectRoot(const std::filesystem::path& extractPath, std::filesystem::path& projectRoot);
    bool moveProjectToFinalLocation(const std::filesystem::path& projectRoot);
    
    // Project adaptation
    bool isCppProject(const std::filesystem::path& projectPath) const;
    bool hasValidStructure(const std::filesystem::path& projectPath) const;
    bool adaptProjectName(const std::filesystem::path& projectPath);
    bool updateProjectConfiguration(const std::filesystem::path& projectPath);
    bool mergeWithTemplateOptions(const std::filesystem::path& projectPath);

    // Member variables
    std::filesystem::path archivePath_;
    std::filesystem::path tempExtractPath_;
    std::filesystem::path finalProjectPath_;
    bool archiveDownloaded_;
    bool archiveExtracted_;
    bool isRemoteSource_;
};
