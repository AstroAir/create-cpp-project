#include "git_repository_template.h"
#include "../utils/core/file_utils.h"
#include "../utils/ui/terminal_utils.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <iostream>

GitRepositoryTemplate::GitRepositoryTemplate(const CliOptions& options)
    : TemplateBase(options), repositoryCloned_(false), authenticationSetup_(false) {

    // Set up paths
    tempClonePath_ = std::filesystem::temp_directory_path() /
                    ("cpp_scaffold_clone_" + std::to_string(std::time(nullptr)));
    finalProjectPath_ = std::filesystem::current_path() / options.projectName;
}

bool GitRepositoryTemplate::create() {
    try {
        spdlog::info("ðŸš€ Creating project from Git repository...");

        // Validate repository URL
        if (!options_.gitRepositoryUrl) {
            spdlog::error("No Git repository URL provided");
            return false;
        }

        if (!utils::GitUtils::isValidGitUrl(*options_.gitRepositoryUrl)) {
            spdlog::error("Invalid Git repository URL: {}", *options_.gitRepositoryUrl);
            return false;
        }

        // Check if target directory already exists
        if (std::filesystem::exists(finalProjectPath_)) {
            spdlog::error("Directory '{}' already exists", finalProjectPath_.string());
            return false;
        }

        // Setup authentication if needed
        if (hasAuthentication()) {
            if (!setupAuthentication()) {
                spdlog::warn("Failed to setup authentication, proceeding without it");
            }
        }

        // Clone repository
        if (!cloneRepository()) {
            spdlog::error("Failed to clone repository");
            return false;
        }
        spdlog::info("âœ?Repository cloned successfully");

        // Process cloned repository
        if (!processClonedRepository()) {
            spdlog::error("Failed to process cloned repository");
            cleanupRepository();
            return false;
        }
        spdlog::info("âœ?Repository processed successfully");

        // Move to final location
        try {
            std::filesystem::rename(tempClonePath_, finalProjectPath_);
            spdlog::info("âœ?Project moved to final location: {}", finalProjectPath_.string());
        } catch (const std::exception& e) {
            spdlog::error("Failed to move project to final location: {}", e.what());
            cleanupRepository();
            return false;
        }

        // Setup additional configurations based on CLI options
        if (!setupProjectFromRepository()) {
            spdlog::warn("Some project setup steps failed, but project was created");
        }

        spdlog::info("\nðŸŽ‰ Project created successfully from Git repository!\n");

        // Print usage instructions
        std::cout << fmt::format("cd {}\n", options_.projectName);
        std::cout << "# Review the project structure and build instructions\n";
        std::cout << "# Check README.md for specific build steps\n";

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error creating project from Git repository: {}", e.what());
        cleanupRepository();
        return false;
    }
}

bool GitRepositoryTemplate::createProjectStructure() {
    // For Git repository template, structure is already created by cloning
    return validateRepositoryStructure();
}

bool GitRepositoryTemplate::createBuildSystem() {
    // Build system files should already exist in the cloned repository
    // We might need to update them based on CLI options
    return updateProjectConfiguration(finalProjectPath_);
}

bool GitRepositoryTemplate::setupPackageManager() {
    // Package manager setup might need to be adapted based on CLI options
    return mergeWithTemplateOptions(finalProjectPath_);
}

bool GitRepositoryTemplate::setupTestFramework() {
    // Test framework might already be configured in the repository
    // Add additional test setup if requested via CLI options
    if (options_.includeTests) {
        // Additional test setup logic here
        spdlog::info("Test framework setup requested - checking existing configuration");
    }
    return true;
}

bool GitRepositoryTemplate::cloneRepository() {
    try {
        std::string repoUrl = getRepositoryUrl();

        // Create temporary directory
        std::filesystem::create_directories(tempClonePath_.parent_path());

        bool success = false;

        if (hasAuthentication()) {
            success = utils::GitUtils::cloneRepositoryWithAuth(
                repoUrl,
                tempClonePath_,
                options_.gitUsername,
                options_.gitPassword,
                options_.sshKeyPath,
                options_.useShallowClone,
                options_.gitBranch,
                options_.gitTag,
                options_.gitCommit
            );
        } else {
            success = utils::GitUtils::cloneRepository(
                repoUrl,
                tempClonePath_,
                options_.useShallowClone,
                options_.gitBranch,
                options_.gitTag,
                options_.gitCommit
            );
        }

        if (success) {
            repositoryCloned_ = true;
            spdlog::info("Repository cloned to temporary location: {}", tempClonePath_.string());
        }

        return success;

    } catch (const std::exception& e) {
        spdlog::error("Error cloning repository: {}", e.what());
        return false;
    }
}

bool GitRepositoryTemplate::processClonedRepository() {
    try {
        // Validate that it's a C++ project
        if (!isCppProject(tempClonePath_)) {
            spdlog::warn("Repository doesn't appear to be a C++ project, proceeding anyway");
        }

        // Validate repository structure
        if (!hasValidStructure(tempClonePath_)) {
            spdlog::warn("Repository structure validation failed, proceeding anyway");
        }

        // Adapt project name if needed
        if (!adaptProjectName(tempClonePath_)) {
            spdlog::warn("Failed to adapt project name");
        }

        // Remove .git directory if not preserving history
        if (!options_.preserveGitHistory) {
            if (utils::GitUtils::removeGitDirectory(tempClonePath_)) {
                spdlog::info("Removed .git directory");
            }
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error processing cloned repository: {}", e.what());
        return false;
    }
}

bool GitRepositoryTemplate::validateRepositoryStructure() {
    return std::filesystem::exists(finalProjectPath_) &&
           std::filesystem::is_directory(finalProjectPath_);
}

bool GitRepositoryTemplate::cleanupRepository() {
    try {
        if (std::filesystem::exists(tempClonePath_)) {
            std::filesystem::remove_all(tempClonePath_);
            spdlog::info("Cleaned up temporary clone directory");
        }
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error cleaning up repository: {}", e.what());
        return false;
    }
}

bool GitRepositoryTemplate::setupProjectFromRepository() {
    try {
        // Initialize new Git repository if requested
        if (options_.initGit && !options_.preserveGitHistory) {
            if (utils::GitUtils::initializeRepository(finalProjectPath_)) {
                spdlog::info("Initialized new Git repository");

                // Configure Git if user info provided
                if (!options_.gitUserName.empty() || !options_.gitUserEmail.empty()) {
                    utils::GitUtils::configureRepository(finalProjectPath_,
                                                        options_.gitUserName,
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
        spdlog::error("Error setting up project from repository: {}", e.what());
        return false;
    }
}

std::string GitRepositoryTemplate::getRepositoryUrl() const {
    return options_.gitRepositoryUrl.value_or("");
}

std::string GitRepositoryTemplate::getTargetBranch() const {
    return options_.gitBranch.value_or("");
}

std::string GitRepositoryTemplate::getTargetTag() const {
    return options_.gitTag.value_or("");
}

std::string GitRepositoryTemplate::getTargetCommit() const {
    return options_.gitCommit.value_or("");
}

std::filesystem::path GitRepositoryTemplate::getTempClonePath() const {
    return tempClonePath_;
}

std::filesystem::path GitRepositoryTemplate::getFinalProjectPath() const {
    return finalProjectPath_;
}

bool GitRepositoryTemplate::hasAuthentication() const {
    return options_.gitUsername.has_value() ||
           options_.gitPassword.has_value() ||
           options_.sshKeyPath.has_value();
}

bool GitRepositoryTemplate::setupAuthentication() {
    // Authentication setup is handled in the GitUtils cloning methods
    authenticationSetup_ = true;
    return true;
}

bool GitRepositoryTemplate::isCppProject(const std::filesystem::path& repoPath) const {
    // Check for common C++ project indicators
    std::vector<std::string> cppIndicators = {
        "CMakeLists.txt", "Makefile", "meson.build", "BUILD", "WORKSPACE",
        "*.cpp", "*.hpp", "*.cc", "*.h", "*.cxx"
    };

    for (const auto& indicator : cppIndicators) {
        if (indicator.find("*") != std::string::npos) {
            // Check for file patterns
            for (const auto& entry : std::filesystem::recursive_directory_iterator(repoPath)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".cpp" || ext == ".hpp" || ext == ".cc" ||
                        ext == ".h" || ext == ".cxx") {
                        return true;
                    }
                }
            }
        } else {
            // Check for specific files
            if (std::filesystem::exists(repoPath / indicator)) {
                return true;
            }
        }
    }

    return false;
}

bool GitRepositoryTemplate::hasValidStructure(const std::filesystem::path& repoPath) const {
    // Basic validation - check if directory is not empty and has some files
    try {
        for (const auto& entry : std::filesystem::directory_iterator(repoPath)) {
            if (entry.is_regular_file() || entry.is_directory()) {
                return true;
            }
        }
    } catch (const std::exception&) {
        return false;
    }

    return false;
}

bool GitRepositoryTemplate::adaptProjectName(const std::filesystem::path& repoPath) {
    (void)repoPath; // TODO: Implement project name adaptation logic
    // This could involve updating CMakeLists.txt, package.json, etc.
    // For now, just log the action
    spdlog::info("Adapting project name to: {}", options_.projectName);
    return true;
}

bool GitRepositoryTemplate::updateProjectConfiguration(const std::filesystem::path& repoPath) {
    (void)repoPath; // TODO: Implement project configuration update logic
    // Update build system configuration based on CLI options
    spdlog::info("Updating project configuration based on CLI options");
    return true;
}

bool GitRepositoryTemplate::mergeWithTemplateOptions(const std::filesystem::path& repoPath) {
    (void)repoPath; // TODO: Implement template options merging logic
    // Merge CLI options with existing project configuration
    spdlog::info("Merging template options with existing project configuration");
    return true;
}
