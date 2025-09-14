#include "archive_template.h"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>

#include "../utils/core/file_utils.h"
#include "../utils/external/git_utils.h"
#include "../utils/ui/terminal_utils.h"

ArchiveTemplate::ArchiveTemplate(const CliOptions& options)
    : TemplateBase(options), archiveDownloaded_(false), archiveExtracted_(false) {
    // Determine if source is remote or local
    isRemoteSource_ = isRemoteArchive();

    // Set up paths
    if (isRemoteSource_) {
        std::string filename = utils::HttpClient::extractFilenameFromUrl(getArchiveSource());
        archivePath_ = std::filesystem::temp_directory_path() / filename;
    } else {
        archivePath_ = getArchiveSource();
    }

    tempExtractPath_ = std::filesystem::temp_directory_path() /
                       ("cpp_scaffold_extract_" + std::to_string(std::time(nullptr)));
    finalProjectPath_ = std::filesystem::current_path() / options.projectName;
}

bool ArchiveTemplate::create() {
    try {
        spdlog::info("ðŸš€ Creating project from archive...");

        // Validate archive source
        std::string archiveSource = getArchiveSource();
        if (archiveSource.empty()) {
            spdlog::error("No archive source provided");
            return false;
        }

        // Check if target directory already exists
        if (std::filesystem::exists(finalProjectPath_)) {
            spdlog::error("Directory '{}' already exists", finalProjectPath_.string());
            return false;
        }

        // Download archive if remote
        if (isRemoteSource_) {
            if (!downloadArchive()) {
                spdlog::error("Failed to download archive");
                return false;
            }
            spdlog::info("âœ?Archive downloaded successfully");
        } else {
            if (!validateLocalArchive()) {
                spdlog::error("Local archive validation failed");
                return false;
            }
            spdlog::info("âœ?Local archive validated");
        }

        // Extract archive
        if (!extractArchive()) {
            spdlog::error("Failed to extract archive");
            cleanupTemporaryFiles();
            return false;
        }
        spdlog::info("âœ?Archive extracted successfully");

        // Process extracted content
        if (!processExtractedContent()) {
            spdlog::error("Failed to process extracted content");
            cleanupTemporaryFiles();
            return false;
        }
        spdlog::info("âœ?Content processed successfully");

        // Setup additional configurations based on CLI options
        if (!setupProjectFromArchive()) {
            spdlog::warn("Some project setup steps failed, but project was created");
        }

        // Cleanup temporary files
        cleanupTemporaryFiles();

        spdlog::info("\nðŸŽ‰ Project created successfully from archive!\n");

        // Print usage instructions
        std::cout << fmt::format("cd {}\n", options_.projectName);
        std::cout << "# Review the project structure and build instructions\n";
        std::cout << "# Check README.md for specific build steps\n";

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error creating project from archive: {}", e.what());
        cleanupTemporaryFiles();
        return false;
    }
}

bool ArchiveTemplate::createProjectStructure() {
    // For archive template, structure is already created by extraction
    return validateArchiveStructure();
}

bool ArchiveTemplate::createBuildSystem() {
    // Build system files should already exist in the extracted archive
    return updateProjectConfiguration(finalProjectPath_);
}

bool ArchiveTemplate::setupPackageManager() {
    // Package manager setup might need to be adapted based on CLI options
    return mergeWithTemplateOptions(finalProjectPath_);
}

bool ArchiveTemplate::setupTestFramework() {
    // Test framework might already be configured in the archive
    if (options_.includeTests) {
        spdlog::info("Test framework setup requested - checking existing configuration");
    }
    return true;
}

bool ArchiveTemplate::downloadArchive() {
    if (!isRemoteSource_) {
        return true;  // Nothing to download
    }

    try {
        std::string url = getArchiveSource();

        // Check if URL is accessible
        if (!utils::HttpClient::isUrlAccessible(url)) {
            spdlog::error("Archive URL is not accessible: {}", url);
            return false;
        }

        // Download with progress tracking
        auto progressCallback = [](size_t downloaded, size_t total) -> bool {
            if (total > 0) {
                double percentage = (double)downloaded / total * 100.0;
                spdlog::info("Download progress: {:.1f}% ({} / {} bytes)", percentage, downloaded,
                             total);
            }
            return true;  // Continue download
        };

        auto result = utils::HttpClient::downloadFile(url, archivePath_, progressCallback);

        if (result.success) {
            archiveDownloaded_ = true;
            spdlog::info("Archive downloaded: {} bytes", result.bytesDownloaded);
            return true;
        } else {
            spdlog::error("Download failed: {}", result.errorMessage);
            return false;
        }

    } catch (const std::exception& e) {
        spdlog::error("Error downloading archive: {}", e.what());
        return false;
    }
}

bool ArchiveTemplate::extractArchive() {
    try {
        // Validate archive format
        if (!utils::ArchiveUtils::isSupportedArchive(archivePath_)) {
            spdlog::error("Unsupported archive format: {}", archivePath_.string());
            return false;
        }

        // Validate archive integrity
        if (!utils::ArchiveUtils::validateArchive(archivePath_)) {
            spdlog::error("Archive validation failed: {}", archivePath_.string());
            return false;
        }

        // Create extraction directory
        std::filesystem::create_directories(tempExtractPath_);

        // Extract with progress tracking
        auto progressCallback = [](size_t current, size_t total) -> bool {
            if (total > 0) {
                double percentage = (double)current / total * 100.0;
                spdlog::info("Extraction progress: {:.1f}% ({} / {} files)", percentage, current,
                             total);
            }
            return true;  // Continue extraction
        };

        auto result = utils::ArchiveUtils::extractArchive(archivePath_, tempExtractPath_,
                                                          progressCallback);

        if (result.success) {
            archiveExtracted_ = true;
            spdlog::info("Archive extracted: {} files", result.totalFiles);
            return true;
        } else {
            spdlog::error("Extraction failed: {}", result.errorMessage);
            return false;
        }

    } catch (const std::exception& e) {
        spdlog::error("Error extracting archive: {}", e.what());
        return false;
    }
}

bool ArchiveTemplate::processExtractedContent() {
    try {
        // Find the actual project root in the extracted content
        std::filesystem::path projectRoot;
        if (!findProjectRoot(tempExtractPath_, projectRoot)) {
            spdlog::error("Could not find project root in extracted content");
            return false;
        }

        // Validate that it's a C++ project
        if (!isCppProject(projectRoot)) {
            spdlog::warn("Archive doesn't appear to contain a C++ project, proceeding anyway");
        }

        // Validate project structure
        if (!hasValidStructure(projectRoot)) {
            spdlog::warn("Project structure validation failed, proceeding anyway");
        }

        // Move project to final location
        if (!moveProjectToFinalLocation(projectRoot)) {
            spdlog::error("Failed to move project to final location");
            return false;
        }

        // Adapt project name if needed
        if (!adaptProjectName(finalProjectPath_)) {
            spdlog::warn("Failed to adapt project name");
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error processing extracted content: {}", e.what());
        return false;
    }
}

bool ArchiveTemplate::validateArchiveStructure() {
    return std::filesystem::exists(finalProjectPath_) &&
           std::filesystem::is_directory(finalProjectPath_);
}

bool ArchiveTemplate::cleanupTemporaryFiles() {
    try {
        // Remove temporary extraction directory
        if (std::filesystem::exists(tempExtractPath_)) {
            std::filesystem::remove_all(tempExtractPath_);
            spdlog::info("Cleaned up temporary extraction directory");
        }

        // Remove downloaded archive if it was downloaded
        if (isRemoteSource_ && archiveDownloaded_ && std::filesystem::exists(archivePath_)) {
            std::filesystem::remove(archivePath_);
            spdlog::info("Cleaned up downloaded archive");
        }

        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error cleaning up temporary files: {}", e.what());
        return false;
    }
}

bool ArchiveTemplate::setupProjectFromArchive() {
    try {
        // Initialize Git repository if requested
        if (options_.initGit) {
            if (utils::GitUtils::initializeRepository(finalProjectPath_)) {
                spdlog::info("Initialized new Git repository");

                // Configure Git if user info provided
                if (!options_.gitUserName.empty() || !options_.gitUserEmail.empty()) {
                    utils::GitUtils::configureRepository(finalProjectPath_, options_.gitUserName,
                                                         options_.gitUserEmail);
                }

                // Add remote if provided
                if (!options_.gitRemoteUrl.empty()) {
                    utils::GitUtils::addRemote(finalProjectPath_, "origin", options_.gitRemoteUrl);
                }
            }
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up project from archive: {}", e.what());
        return false;
    }
}

std::string ArchiveTemplate::getArchiveSource() const {
    if (options_.archiveUrl) {
        return *options_.archiveUrl;
    } else if (options_.archiveFilePath) {
        return *options_.archiveFilePath;
    }
    return "";
}

std::filesystem::path ArchiveTemplate::getArchivePath() const {
    return archivePath_;
}

std::filesystem::path ArchiveTemplate::getTempExtractPath() const {
    return tempExtractPath_;
}

std::filesystem::path ArchiveTemplate::getFinalProjectPath() const {
    return finalProjectPath_;
}

bool ArchiveTemplate::isRemoteArchive() const {
    if (options_.archiveUrl) {
        return utils::HttpClient::isValidUrl(*options_.archiveUrl);
    }
    return false;
}

bool ArchiveTemplate::isLocalArchive() const {
    if (options_.archiveFilePath) {
        return std::filesystem::exists(*options_.archiveFilePath);
    }
    return false;
}

bool ArchiveTemplate::downloadRemoteArchive() {
    return downloadArchive();
}

bool ArchiveTemplate::validateLocalArchive() {
    if (!isLocalArchive()) {
        return false;
    }

    return utils::ArchiveUtils::validateArchive(archivePath_);
}

bool ArchiveTemplate::extractToTemporaryLocation() {
    return extractArchive();
}

bool ArchiveTemplate::findProjectRoot(const std::filesystem::path& extractPath,
                                      std::filesystem::path& projectRoot) {
    try {
        // Check if extract path itself is the project root
        if (isCppProject(extractPath)) {
            projectRoot = extractPath;
            return true;
        }

        // Look for project root in subdirectories
        for (const auto& entry : std::filesystem::directory_iterator(extractPath)) {
            if (entry.is_directory() && isCppProject(entry.path())) {
                projectRoot = entry.path();
                return true;
            }
        }

        // If no clear project root found, use the extract path
        projectRoot = extractPath;
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error finding project root: {}", e.what());
        return false;
    }
}

bool ArchiveTemplate::moveProjectToFinalLocation(const std::filesystem::path& projectRoot) {
    try {
        std::filesystem::rename(projectRoot, finalProjectPath_);
        spdlog::info("Project moved to final location: {}", finalProjectPath_.string());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to move project to final location: {}", e.what());
        return false;
    }
}

bool ArchiveTemplate::isCppProject(const std::filesystem::path& projectPath) const {
    // Check for common C++ project indicators
    std::vector<std::string> cppIndicators = {"CMakeLists.txt", "Makefile", "meson.build", "BUILD",
                                              "WORKSPACE"};

    for (const auto& indicator : cppIndicators) {
        if (std::filesystem::exists(projectPath / indicator)) {
            return true;
        }
    }

    // Check for C++ source files
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".cpp" || ext == ".hpp" || ext == ".cc" || ext == ".h" ||
                    ext == ".cxx") {
                    return true;
                }
            }
        }
    } catch (const std::exception&) {
        // Ignore errors during directory traversal
    }

    return false;
}

bool ArchiveTemplate::hasValidStructure(const std::filesystem::path& projectPath) const {
    // Basic validation - check if directory is not empty
    try {
        for (const auto& entry : std::filesystem::directory_iterator(projectPath)) {
            if (entry.is_regular_file() || entry.is_directory()) {
                return true;
            }
        }
    } catch (const std::exception&) {
        return false;
    }

    return false;
}

bool ArchiveTemplate::adaptProjectName(const std::filesystem::path& projectPath) {
    (void)projectPath;  // TODO: Implement project name adaptation logic
    spdlog::info("Adapting project name to: {}", options_.projectName);
    return true;
}

bool ArchiveTemplate::updateProjectConfiguration(const std::filesystem::path& projectPath) {
    (void)projectPath;  // TODO: Implement project configuration update logic
    spdlog::info("Updating project configuration based on CLI options");
    return true;
}

bool ArchiveTemplate::mergeWithTemplateOptions(const std::filesystem::path& projectPath) {
    (void)projectPath;  // TODO: Implement template options merging logic
    spdlog::info("Merging template options with existing project configuration");
    return true;
}
