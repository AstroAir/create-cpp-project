#include "template_base.h"

#include <spdlog/spdlog.h>

#include <iostream>

#include "../config/ci_config.h"
#include "../config/code_style_config.h"
#include "../config/doc_config.h"
#include "../config/editor_config.h"
#include "../utils/core/file_utils.h"
#include "../utils/core/system_utils.h"
#include "../utils/external/git_utils.h"
#include "../utils/ui/progress_indicator.h"
#include "../utils/ui/terminal_utils.h"

using namespace cli_enums;
using namespace utils;

// Constructor with resource management initialization
TemplateBase::TemplateBase(const CliOptions& options)
    : resourceManager_(std::make_unique<ResourceManager>()), options_(options) {
    resourceManager_->startTime = std::chrono::steady_clock::now();

    // Initialize progress tracking
    currentProgress_ = progress::templateCreation(options_.projectName);

    spdlog::debug("TemplateBase initialized for project: {}", options_.projectName);
}

// Enhanced destructor with cleanup
TemplateBase::~TemplateBase() {
    if (resourceManager_) {
        resourceManager_->cleanup();
    }
    spdlog::debug("TemplateBase destroyed");
}

// Resource Manager Implementation
void TemplateBase::ResourceManager::addCreatedDirectory(const std::filesystem::path& path) {
    createdDirectories.push_back(path);
    spdlog::debug("Registered created directory: {}", path.string());
}

void TemplateBase::ResourceManager::addCreatedFile(const std::filesystem::path& path) {
    createdFiles.push_back(path);
    spdlog::debug("Registered created file: {}", path.string());
}

void TemplateBase::ResourceManager::addExecutedCommand(const std::string& command) {
    executedCommands.push_back(command);
    spdlog::debug("Registered executed command: {}", command);
}

bool TemplateBase::ResourceManager::rollbackChanges() {
    bool success = true;

    // Remove created files (in reverse order)
    for (auto it = createdFiles.rbegin(); it != createdFiles.rend(); ++it) {
        try {
            if (std::filesystem::exists(*it)) {
                std::filesystem::remove(*it);
                spdlog::debug("Rolled back file: {}", it->string());
            }
        } catch (const std::exception& e) {
            spdlog::error("Failed to rollback file {}: {}", it->string(), e.what());
            success = false;
        }
    }

    // Remove created directories (in reverse order)
    for (auto it = createdDirectories.rbegin(); it != createdDirectories.rend(); ++it) {
        try {
            if (std::filesystem::exists(*it) && std::filesystem::is_empty(*it)) {
                std::filesystem::remove(*it);
                spdlog::debug("Rolled back directory: {}", it->string());
            }
        } catch (const std::exception& e) {
            spdlog::error("Failed to rollback directory {}: {}", it->string(), e.what());
            success = false;
        }
    }

    return success;
}

void TemplateBase::ResourceManager::cleanup() {
    createdDirectories.clear();
    createdFiles.clear();
    executedCommands.clear();
}

// Creation method with error handling
bool TemplateBase::createWithEnhancedHandling() {
    try {
        // Pre-creation validation
        if (!validateProjectConfiguration()) {
            reportTemplateError("Configuration Validation", "Project configuration is invalid",
                                "Please check your project settings and try again");
            return false;
        }

        if (!validateSystemRequirements()) {
            reportTemplateError("System Requirements", "System requirements not met",
                                "Please ensure all required tools are installed");
            return false;
        }

        if (!validateDiskSpace()) {
            reportTemplateError("Disk Space", "Insufficient disk space",
                                "Please free up disk space and try again");
            return false;
        }

        // Start progress tracking
        startProgressStep("Project Creation", "Initializing project creation process");

        // Call the derived class implementation
        bool result = create();

        if (result) {
            completeProgressStep("Project created successfully");
            spdlog::info("Project '{}' created successfully", options_.projectName);
        } else {
            failProgressStep("Project creation failed");
            cleanupOnFailure();
        }

        return result;

    } catch (const std::exception& e) {
        reportTemplateError("Unexpected Error", e.what(), "Please report this issue");
        failProgressStep(std::string("Unexpected error: ") + e.what());
        cleanupOnFailure();
        return false;
    }
}

// Enhanced file operations with comprehensive error handling
bool TemplateBase::createDirectoryWithValidation(const std::filesystem::path& path) {
    try {
        bool result = FileUtils::createDirectory(path.string());
        if (!result) {
            reportTemplateError("Directory Creation",
                                "Failed to create directory: " + path.string(),
                                "Check permissions and disk space");
            return false;
        }

        registerCreatedResource(path, true);
        spdlog::debug("Created directory: {}", path.string());
        return true;

    } catch (const std::exception& e) {
        reportTemplateError("Directory Creation Exception", e.what(),
                            "Check file system permissions");
        return false;
    }
}

bool TemplateBase::createFileWithValidation(const std::filesystem::path& path,
                                            const std::string& content) {
    try {
        // Ensure parent directory exists
        auto parentDir = path.parent_path();
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            if (!createDirectoryWithValidation(parentDir)) {
                return false;
            }
        }

        bool result = FileUtils::writeToFile(path.string(), content);
        if (!result) {
            reportTemplateError("File Creation", "Failed to create file: " + path.string(),
                                "Check permissions and disk space");
            return false;
        }

        registerCreatedResource(path, false);
        spdlog::debug("Created file: {}", path.string());
        return true;

    } catch (const std::exception& e) {
        reportTemplateError("File Creation Exception", e.what(), "Check file system permissions");
        return false;
    }
}

// Additional enhanced file operations
bool TemplateBase::copyFileWithValidation(const std::filesystem::path& source,
                                          const std::filesystem::path& dest) {
    try {
        // Validate source file
        if (!std::filesystem::exists(source)) {
            reportTemplateError("File Copy", "Source file does not exist: " + source.string(),
                                "Check the source file path");
            return false;
        }

        // Ensure destination directory exists
        auto destDir = dest.parent_path();
        if (!destDir.empty() && !std::filesystem::exists(destDir)) {
            if (!createDirectoryWithValidation(destDir)) {
                return false;
            }
        }

        bool result = FileUtils::copyFile(source.string(), dest.string());
        if (!result) {
            reportTemplateError(
                    "File Copy",
                    "Failed to copy file from " + source.string() + " to " + dest.string(),
                    "Check permissions and disk space");
            return false;
        }

        registerCreatedResource(dest, false);
        spdlog::debug("Copied file: {} -> {}", source.string(), dest.string());
        return true;

    } catch (const std::exception& e) {
        reportTemplateError("File Copy Exception", e.what(), "Check file system permissions");
        return false;
    }
}

bool TemplateBase::executeCommandWithValidation(const std::string& command,
                                                const std::filesystem::path& workingDir) {
    try {
        spdlog::debug("Executing command: {} in directory: {}", command, workingDir.string());

        // TODO: Implement command execution with proper error handling
        // This would integrate with system_utils or a command execution utility
        resourceManager_->addExecutedCommand(command);

        return true;

    } catch (const std::exception& e) {
        reportTemplateError("Command Execution Exception", e.what(),
                            "Check command syntax and system permissions");
        return false;
    }
}

// Validation helper methods
bool TemplateBase::validateProjectConfiguration() {
    std::string error;

    // Basic project name validation
    if (options_.projectName.empty()) {
        reportTemplateError("Project Name Validation", "Project name cannot be empty",
                            "Provide a valid project name");
        return false;
    }

    // Check if project directory already exists
    if (std::filesystem::exists(options_.projectName)) {
        reportTemplateError("Project Directory",
                            "Directory already exists: " + options_.projectName,
                            "Choose a different project name or remove the existing directory");
        return false;
    }

    return true;
}

bool TemplateBase::validateSystemRequirements() {
    // Check for required tools based on template type and build system
    std::vector<std::string> requiredTools;

    // Add tools based on build system
    if (options_.buildSystem == BuildSystem::CMake) {
        requiredTools.push_back("cmake");
    }

    // Add tools based on package manager
    if (options_.packageManager == PackageManager::Vcpkg) {
        requiredTools.push_back("vcpkg");
    } else if (options_.packageManager == PackageManager::Conan) {
        requiredTools.push_back("conan");
    }

    // Check if Git is required and available
    if (options_.initGit) {
        requiredTools.push_back("git");
    }

    // Validate each required tool
    for (const auto& tool : requiredTools) {
        if (!FileUtils::commandExists(tool)) {
            reportTemplateError("Missing Tool", "Required tool not found: " + tool,
                                "Please install " + tool + " and ensure it's in your PATH");
            return false;
        }
    }

    return true;
}

bool TemplateBase::validateDiskSpace(size_t requiredBytes) {
    try {
        auto space = std::filesystem::space(".");
        if (space.available < requiredBytes) {
            reportTemplateError(
                    "Insufficient Disk Space",
                    "Available: " + std::to_string(space.available / (1024 * 1024)) + "MB, " +
                            "Required: " + std::to_string(requiredBytes / (1024 * 1024)) + "MB",
                    "Free up disk space and try again");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        spdlog::warn("Could not check disk space: {}", e.what());
        return true;  // Don't fail if we can't check
    }
}

// Progress tracking helper methods
void TemplateBase::startProgressStep(const std::string& stepName, const std::string& description) {
    if (currentProgress_) {
        currentProgress_->setMessage(stepName + ": " + description);
    }
    spdlog::info("Starting step: {}", stepName);
}

void TemplateBase::updateProgress(double progress, const std::string& message) {
    if (currentProgress_) {
        currentProgress_->update(progress, message);
    }
}

void TemplateBase::completeProgressStep(const std::string& message) {
    if (currentProgress_) {
        currentProgress_->setMessage(message);
    }
    spdlog::info("Completed step: {}", message);
}

void TemplateBase::failProgressStep(const std::string& error) {
    if (currentProgress_) {
        currentProgress_->setMessage("Failed: " + error);
    }
    spdlog::error("Step failed: {}", error);
}

// Error handling helper methods
void TemplateBase::reportTemplateError(const std::string& operation, const std::string& details,
                                       const std::string& suggestion) {
    spdlog::error("Template Error in {}: {} - Suggestion: {}", operation, details, suggestion);
}

bool TemplateBase::handleFileSystemError(const std::filesystem::path& path,
                                         const std::string& operation) {
    spdlog::error("File system error for path {}: {}", path.string(), operation);
    return false;
}

bool TemplateBase::attemptErrorRecovery(const std::string& errorCode) {
    spdlog::info("Attempting error recovery for: {}", errorCode);
    return false;  // Basic implementation - no recovery
}

// Resource cleanup helper methods
void TemplateBase::registerCreatedResource(const std::filesystem::path& path, bool isDirectory) {
    if (isDirectory) {
        resourceManager_->addCreatedDirectory(path);
    } else {
        resourceManager_->addCreatedFile(path);
    }
}

bool TemplateBase::cleanupOnFailure() {
    spdlog::warn("Cleaning up resources due to failure");
    return resourceManager_->rollbackChanges();
}

bool TemplateBase::initializeGit(const std::string& projectPath) {
    if (!options_.initGit) {
        return true;
    }

    std::cout << "?? ��ʼ��Git�ֿ�...\n";

    // Use enhanced Git utilities
    std::filesystem::path projectDir(projectPath);

    // Initialize Git repository
    if (!utils::GitUtils::initializeRepository(projectDir)) {
        std::cerr << "? Git�ֿ��ʼ��ʧ��\n";
        return false;
    }

    // Create comprehensive .gitignore
    std::string templateType = std::string(to_string(options_.templateType));
    std::string buildSystem = std::string(to_string(options_.buildSystem));
    std::string packageManager = std::string(to_string(options_.packageManager));

    if (!utils::GitUtils::createGitignore(projectDir, templateType, buildSystem, packageManager)) {
        std::cerr << "? ����.gitignore�ļ�ʧ��\n";
        return false;
    }

    // Create .gitattributes
    if (!utils::GitUtils::createGitAttributes(projectDir)) {
        std::cerr << "? ����.gitattributes�ļ�ʧ��\n";
        return false;
    }

    // Enhanced repository configuration
    std::string gitWorkflow = std::string(to_string(options_.gitWorkflow));
    std::string licenseType = std::string(to_string(options_.licenseType));

    if (!utils::GitUtils::configureRepositoryAdvanced(projectDir, options_.gitUserName,
                                                      options_.gitUserEmail, options_.gitRemoteUrl,
                                                      options_.setupGitHooks)) {
        std::cerr << "? Git�ֿ�߼�����ʧ��\n";
    }

    // Setup Git workflow
    if (gitWorkflow != "none") {
        if (!utils::GitUtils::setupGitWorkflow(projectDir, gitWorkflow)) {
            std::cerr << "? Git����������ʧ��\n";
        }
    }

    // Create branches based on strategy
    std::string branchStrategy = std::string(to_string(options_.gitBranchStrategy));
    if (!utils::GitUtils::createBranchesFromStrategy(projectDir, branchStrategy,
                                                     options_.gitBranches)) {
        std::cerr << "? Git��֧����ʧ��\n";
    }

    // Create license file
    if (licenseType != "none") {
        if (!utils::GitUtils::createLicenseFile(
                    projectDir, licenseType, options_.projectName,
                    options_.gitUserName.empty() ? "Project Author" : options_.gitUserName)) {
            std::cerr << "? ����֤�ļ�����ʧ��\n";
        }
    }

    // Create code quality configurations if enabled
    if (options_.includeCodeStyleTools) {
        utils::CodeQualityTools::createClangFormatConfig(projectDir);
        utils::CodeQualityTools::createClangTidyConfig(projectDir);
        utils::CodeQualityTools::createEditorConfig(projectDir);
    }

    // Create initial README if it doesn't exist
    std::string readmePath = utils::FileUtils::combinePath(projectPath, "README.md");
    if (!utils::FileUtils::fileExists(readmePath)) {
        if (!utils::FileUtils::writeToFile(
                    readmePath,
                    "# " + options_.projectName + "\n\n��ʼ��Ŀ�ṹ��CPP-Scaffold������\n")) {
            std::cerr << "? ����README.md�ļ�ʧ��\n";
        }
    }

    // Create initial commit if requested
    if (options_.createInitialCommit) {
        if (!utils::GitUtils::createInitialCommit(projectDir, "Initial commit")) {
            std::cerr << "? ������ʼ�ύʧ��\n";
            return false;
        }
    }

    std::cout << "? Git�ֿ��ʼ�����\n";
    return true;
}

std::string TemplateBase::getGitignoreContent() {
    return R"(# Build directories
build/
bin/
lib/
out/
cmake-build-*/

# Dependency directories
vcpkg_installed/
conan/

# IDE files
.vs/
.vscode/
.idea/
*.swp
*~

# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
install_manifest.txt

# Bazel files
bazel-*

# Generated files
compile_commands.json

# �����������
.vcpkg/
conanbuildinfo.*
conaninfo.*
graph_info.json

# �������
Testing/
CTestTestfile.cmake
)";
}

// ���ӻ�ȡ��Ŀ����֤���ݵĸ�������
std::string TemplateBase::getLicenseContent(const std::string& projectName) {
    // ����һ���򵥵�MIT����֤ģ��
    return R"(MIT License

Copyright (c) )" +
           std::to_string(2025) + R"( )" + projectName + R"(

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
)";
}

// ��������֤�ļ�
bool TemplateBase::createLicense(const std::string& projectPath) {
    return utils::FileUtils::writeToFile(utils::FileUtils::combinePath(projectPath, "LICENSE"),
                                         getLicenseContent(options_.projectName));
}

// �����յ�.clang-format�ļ�
bool TemplateBase::createClangFormat(const std::string& projectPath) {
    return utils::FileUtils::writeToFile(
            utils::FileUtils::combinePath(projectPath, ".clang-format"),
            R"(BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AccessModifierOffset: -4
NamespaceIndentation: All
BreakBeforeBraces: Allman
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AlwaysBreakTemplateDeclarations: Yes
)");
}

// ����.gitattributes�ļ�
bool TemplateBase::createGitAttributes(const std::string& projectPath) {
    return utils::FileUtils::writeToFile(
            utils::FileUtils::combinePath(projectPath, ".gitattributes"),
            R"(# ����Ĭ����β���
* text=auto

# C/C++ Դ�ļ�
*.c text
*.cc text
*.cxx text
*.cpp text
*.c++ text
*.hpp text
*.h text
*.h++ text
*.hh text

# ����Ϊ�������ļ�������βת��
*.png binary
*.jpg binary
*.jpeg binary
*.gif binary
*.ico binary
*.mov binary
*.mp4 binary
*.mp3 binary
*.flv binary
*.fla binary
*.swf binary
*.gz binary
*.zip binary
*.7z binary
*.ttf binary
*.eot binary
*.woff binary
*.pyc binary
*.pdf binary
*.dll binary
*.so binary
*.dylib binary
*.exe binary
*.lib binary
*.a binary
)");
}

// ����CI/CD����
bool TemplateBase::setupCICD(const std::string& projectPath) {
    if (options_.ciOptions.empty()) {
        return true;  // No CI/CD systems selected, return success directly
    }

    std::cout << "Setting up CI/CD configuration...\n";

    bool result = CIConfig::createCIConfigs(projectPath, options_.ciOptions, options_);

    if (result) {
        std::cout << "CI/CD configuration created successfully\n";
    } else {
        std::cerr << "CI/CD configuration setup failed\n";
    }

    return result;
}

// ���ñ༭������
bool TemplateBase::setupEditorConfig(const std::string& projectPath) {
    if (options_.editorOptions.empty()) {
        return true;  // No editor configuration selected, return success directly
    }

    std::cout << "Setting up editor configuration...\n";

    bool result =
            EditorConfigManager::createEditorConfigs(projectPath, options_.editorOptions, options_);

    if (result) {
        std::cout << "Editor configuration created successfully\n";
    } else {
        std::cerr << "Editor configuration setup failed\n";
    }

    return result;
}

// Setup code style configuration
bool TemplateBase::setupCodeStyleConfig(const std::string& projectPath) {
    if (!options_.includeCodeStyleTools) {
        return true;  // No code style tools selected, return success directly
    }

    std::cout << "Setting up code style configuration...\n";

    bool result = CodeStyleConfig::createCodeStyleConfig(projectPath, options_);

    if (result) {
        std::cout << "Code style configuration created successfully\n";
    } else {
        std::cerr << "Code style configuration setup failed\n";
    }

    return result;
}

// Setup documentation configuration
bool TemplateBase::setupDocConfig(const std::string& projectPath) {
    if (!options_.includeDocumentation) {
        return true;  // No documentation selected, return success directly
    }

    std::cout << "Setting up documentation configuration...\n";

    bool result = DocConfig::createDocConfig(projectPath, options_);

    if (result) {
        std::cout << "Documentation configuration created successfully\n";
    } else {
        std::cerr << "Documentation configuration setup failed\n";
    }

    return result;
}

// ��ӡ��Ŀ�������ʹ��ָ��
void TemplateBase::printUsageGuide() {
    std::cout << "\n?? ��Ŀʹ��ָ��:\n\n";

    std::cout << "1. ������ĿĿ¼:\n";
    std::cout << "   cd " << options_.projectName << "\n\n";

    std::cout << "2. ������Ŀ:\n";
    if (to_string(options_.buildSystem) == "cmake") {
        std::cout << "   mkdir build && cd build\n";
        std::cout << "   cmake ..\n";
        std::cout << "   cmake --build .\n";
    } else if (to_string(options_.buildSystem) == "meson") {
        std::cout << "   meson setup build\n";
        std::cout << "   cd build\n";
        std::cout << "   meson compile\n";
    } else if (to_string(options_.buildSystem) == "bazel") {
        std::cout << "   bazel build //...\n";
    } else if (to_string(options_.buildSystem) == "xmake") {
        std::cout << "   xmake\n";
    } else if (to_string(options_.buildSystem) == "premake") {
        std::cout << "   premake5 gmake\n";
        std::cout << "   make\n";
    }
    std::cout << "\n";

    if (options_.includeTests) {
        std::cout << "3. ���в���:\n";
        if (to_string(options_.buildSystem) == "cmake") {
            std::cout << "   cd build\n";
            std::cout << "   ctest\n";
        } else if (to_string(options_.buildSystem) == "meson") {
            std::cout << "   cd build\n";
            std::cout << "   meson test\n";
        } else if (to_string(options_.buildSystem) == "bazel") {
            std::cout << "   bazel test //...\n";
        } else if (to_string(options_.buildSystem) == "xmake") {
            std::cout << "   xmake test\n";
        } else if (to_string(options_.buildSystem) == "premake") {
            std::cout << "   bin/Debug/" << options_.projectName << "_tests\n";
        }
        std::cout << "\n";
    }

    if (to_string(options_.packageManager) != "none") {
        std::cout << "4. ������: \n";
        if (to_string(options_.packageManager) == "vcpkg") {
            std::cout << "   vcpkg��װ��������vcpkg.json������\n";
        } else if (to_string(options_.packageManager) == "conan") {
            std::cout << "   �ڹ�����Ŀǰ����:\n";
            std::cout << "   conan install . --build=missing\n";
        }
        std::cout << "\n";
    }

    if (!options_.ciOptions.empty()) {
        std::cout << "5. CI/CD����: \n";
        std::cout << "   ��Ϊ����CI/CDϵͳ��������:\n";
        for (const auto& ci : options_.ciOptions) {
            std::cout << "   - " << to_string(ci) << "\n";
        }
        std::cout << "\n";
    }

    if (options_.includeCodeStyleTools) {
        std::cout << "6. �����񹤾�: \n";
        std::cout << "   ��Ŀ������clang-format��clang-tidy��EditorConfig\n";
        std::cout << "\n";
    }

    if (options_.includeDocumentation) {
        std::cout << "7. �ĵ�����: \n";
        std::cout << "   ʹ���������������ĵ�:\n";
        std::cout << "   cd docs\n";
#ifdef _WIN32
        std::cout << "   .\\generate_docs.bat\n";
#else
        std::cout << "   ./generate_docs.sh\n";
#endif
        std::cout << "\n";
    }

    std::cout << "ף�������! ??\n";
}

// ִ�д�����Ĳ���
bool TemplateBase::executePostCreationActions() {
    // Temporarily disabled due to namespace conflicts
    // utils::PostCreationActions& actions = utils::PostCreationActions::getInstance();
    // return actions.executeAll(options_);
    return true;
}
