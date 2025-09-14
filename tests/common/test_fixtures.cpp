#include "test_fixtures.h"
#include "utils/core/file_utils.h"
#include <fstream>
#include <random>
#include <sstream>
#include <cstring>

namespace test_utils {

// TempDirectoryFixture Implementation
void TempDirectoryFixture::SetUp() {
    // Generate unique test directory name
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);

    std::string testDirName = "cpp_scaffold_test_" + std::to_string(dis(gen));
    testDir = std::filesystem::temp_directory_path() / testDirName;

    // Store original directory
    originalDir = std::filesystem::current_path();

    // Create and change to test directory
    std::filesystem::create_directories(testDir);
    std::filesystem::current_path(testDir);
}

void TempDirectoryFixture::TearDown() {
    // Restore original directory
    std::filesystem::current_path(originalDir);

    // Clean up test directory
    if (std::filesystem::exists(testDir)) {
        std::filesystem::remove_all(testDir);
    }
}

std::filesystem::path TempDirectoryFixture::createSubDir(const std::string& subdir) {
    std::filesystem::path subdirPath = testDir / subdir;
    std::filesystem::create_directories(subdirPath);
    return subdirPath;
}

std::filesystem::path TempDirectoryFixture::createTestFile(const std::string& filename, const std::string& content) {
    std::filesystem::path filePath = testDir / filename;

    // Create parent directories if needed
    std::filesystem::create_directories(filePath.parent_path());

    std::ofstream file(filePath);
    if (file.is_open()) {
        file << content;
        file.close();
    }

    return filePath;
}

// ProjectFixture Implementation
void ProjectFixture::SetUp() {
    TempDirectoryFixture::SetUp();

    projectName = "test_project";
    projectDir = testDir / projectName;

    // Create basic project structure
    std::filesystem::create_directories(projectDir);
    std::filesystem::create_directories(projectDir / "src");
    std::filesystem::create_directories(projectDir / "include");
    std::filesystem::create_directories(projectDir / "tests");
    std::filesystem::create_directories(projectDir / "docs");

    // Create basic files
    createTestFile(projectName + "/README.md", "# " + projectName + "\n\nTest project for unit testing.");
    createTestFile(projectName + "/CMakeLists.txt", "cmake_minimum_required(VERSION 3.20)\nproject(" + projectName + ")");
    createTestFile(projectName + "/.gitignore", "build/\n*.o\n*.exe");
}

bool ProjectFixture::verifyBasicProjectStructure() const {
    return std::filesystem::exists(projectDir) &&
           std::filesystem::exists(projectDir / "src") &&
           std::filesystem::exists(projectDir / "include") &&
           std::filesystem::exists(projectDir / "README.md") &&
           std::filesystem::exists(projectDir / "CMakeLists.txt");
}

bool ProjectFixture::verifyFileContains(const std::filesystem::path& filePath, const std::string& expectedContent) const {
    if (!std::filesystem::exists(filePath)) {
        return false;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content.find(expectedContent) != std::string::npos;
}

// ConfigFixture Implementation
void ConfigFixture::SetUp() {
    TempDirectoryFixture::SetUp();

    configDir = testDir / "config";
    std::filesystem::create_directories(configDir);
}

std::filesystem::path ConfigFixture::createConfigFile(const std::string& configName, const std::string& content) {
    std::filesystem::path configPath = configDir / (configName + ".json");

    std::ofstream file(configPath);
    if (file.is_open()) {
        file << content;
        file.close();
    }

    return configPath;
}

// CliFixture Implementation
void CliFixture::SetUp() {
    TempDirectoryFixture::SetUp();
}

std::pair<int, char**> CliFixture::createArgv(const std::vector<std::string>& args) {
    // Clean up any previous allocations
    cleanupArgv();

    allocatedArgs.reserve(args.size());

    for (const auto& arg : args) {
        char* argCopy = new char[arg.length() + 1];
        std::strcpy(argCopy, arg.c_str());
        allocatedArgs.push_back(argCopy);
    }

    return {static_cast<int>(allocatedArgs.size()), allocatedArgs.data()};
}

void CliFixture::cleanupArgv() {
    for (char* arg : allocatedArgs) {
        delete[] arg;
    }
    allocatedArgs.clear();
}

// TemplateFixture Implementation
void TemplateFixture::SetUp() {
    ProjectFixture::SetUp();
}

} // namespace test_utils
