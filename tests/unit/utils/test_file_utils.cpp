#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "utils/core/file_utils.h"

using namespace utils;

class FileUtilsTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "cpp_scaffold_test";
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;
};

TEST_F(FileUtilsTest, CreateDirectory) {
    std::string newDir = (testDir / "new_directory").string();
    EXPECT_TRUE(FileUtils::createDirectory(newDir));
    EXPECT_TRUE(std::filesystem::exists(newDir));
    EXPECT_TRUE(std::filesystem::is_directory(newDir));
}

TEST_F(FileUtilsTest, CreateNestedDirectory) {
    std::string nestedDir = (testDir / "level1" / "level2" / "level3").string();
    EXPECT_TRUE(FileUtils::createDirectory(nestedDir));
    EXPECT_TRUE(std::filesystem::exists(nestedDir));
    EXPECT_TRUE(std::filesystem::is_directory(nestedDir));
}

TEST_F(FileUtilsTest, WriteAndReadFile) {
    std::string filePath = (testDir / "test_file.txt").string();
    std::string content = "Hello, World!\nThis is a test file.";

    EXPECT_TRUE(FileUtils::writeToFile(filePath, content));
    EXPECT_TRUE(FileUtils::fileExists(filePath));

    std::string readContent = FileUtils::readFromFile(filePath);
    EXPECT_EQ(content, readContent);
}

TEST_F(FileUtilsTest, FileExists) {
    std::string existingFile = (testDir / "existing.txt").string();
    std::string nonExistingFile = (testDir / "non_existing.txt").string();

    // Create a file
    std::ofstream file(existingFile);
    file << "test content";
    file.close();

    EXPECT_TRUE(FileUtils::fileExists(existingFile));
    EXPECT_FALSE(FileUtils::fileExists(nonExistingFile));
}

TEST_F(FileUtilsTest, DirectoryExists) {
    std::string existingDir = (testDir / "existing_dir").string();
    std::string nonExistingDir = (testDir / "non_existing_dir").string();

    std::filesystem::create_directory(existingDir);

    EXPECT_TRUE(FileUtils::directoryExists(existingDir));
    EXPECT_FALSE(FileUtils::directoryExists(nonExistingDir));
}

TEST_F(FileUtilsTest, CopyFile) {
    std::string sourceFile = (testDir / "source.txt").string();
    std::string destFile = (testDir / "destination.txt").string();
    std::string content = "File content to copy";

    FileUtils::writeToFile(sourceFile, content);
    EXPECT_TRUE(FileUtils::copyFile(sourceFile, destFile));
    EXPECT_TRUE(FileUtils::fileExists(destFile));

    std::string copiedContent = FileUtils::readFromFile(destFile);
    EXPECT_EQ(content, copiedContent);
}

TEST_F(FileUtilsTest, CopyDirectory) {
    std::string sourceDir = (testDir / "source_dir").string();
    std::string destDir = (testDir / "dest_dir").string();

    // Create source directory with files
    FileUtils::createDirectory(sourceDir);
    FileUtils::writeToFile(FileUtils::combinePath(sourceDir, "file1.txt"), "Content 1");
    FileUtils::writeToFile(FileUtils::combinePath(sourceDir, "file2.txt"), "Content 2");

    EXPECT_TRUE(FileUtils::copyDirectory(sourceDir, destDir));
    EXPECT_TRUE(FileUtils::directoryExists(destDir));
    EXPECT_TRUE(FileUtils::fileExists(FileUtils::combinePath(destDir, "file1.txt")));
    EXPECT_TRUE(FileUtils::fileExists(FileUtils::combinePath(destDir, "file2.txt")));
}

TEST_F(FileUtilsTest, GetFileName) {
    EXPECT_EQ("file.txt", FileUtils::getFileName("/path/to/file.txt"));
    EXPECT_EQ("file.txt", FileUtils::getFileName("file.txt"));
    EXPECT_EQ("file", FileUtils::getFileName("/path/to/file"));
}

TEST_F(FileUtilsTest, GetDirectoryName) {
    EXPECT_EQ("/path/to", FileUtils::getDirectoryName("/path/to/file.txt"));
    EXPECT_EQ(".", FileUtils::getDirectoryName("file.txt"));
}

TEST_F(FileUtilsTest, GetFileExtension) {
    EXPECT_EQ(".txt", FileUtils::getFileExtension("file.txt"));
    EXPECT_EQ(".cpp", FileUtils::getFileExtension("/path/to/source.cpp"));
    EXPECT_EQ("", FileUtils::getFileExtension("file_without_extension"));
}

TEST_F(FileUtilsTest, CombinePath) {
    // Normalize path separators for cross-platform compatibility
    auto normalize = [](const std::string& path) {
        std::string result = path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    };

    EXPECT_EQ("path/to/file.txt", normalize(FileUtils::combinePath("path/to", "file.txt")));
    EXPECT_EQ("path/file.txt", normalize(FileUtils::combinePath("path", "file.txt")));
}

TEST_F(FileUtilsTest, ListFiles) {
    std::string dir = (testDir / "list_test").string();
    FileUtils::createDirectory(dir);

    // Create some test files
    FileUtils::writeToFile(FileUtils::combinePath(dir, "file1.txt"), "content1");
    FileUtils::writeToFile(FileUtils::combinePath(dir, "file2.cpp"), "content2");
    FileUtils::writeToFile(FileUtils::combinePath(dir, "file3.h"), "content3");

    auto files = FileUtils::listFiles(dir);
    EXPECT_EQ(3, files.size());

    // Check that all files are present (order may vary)
    std::set<std::string> fileSet(files.begin(), files.end());
    EXPECT_TRUE(fileSet.count("file1.txt") > 0);
    EXPECT_TRUE(fileSet.count("file2.cpp") > 0);
    EXPECT_TRUE(fileSet.count("file3.h") > 0);
}

TEST_F(FileUtilsTest, WriteToNonExistentDirectory) {
    std::string filePath = (testDir / "non_existent" / "file.txt").string();
    std::string content = "test content";

    // Enhanced writeToFile should now create directories automatically
    EXPECT_TRUE(FileUtils::writeToFile(filePath, content));

    // Verify the file was created and directory exists
    EXPECT_TRUE(FileUtils::fileExists(filePath));
    EXPECT_TRUE(FileUtils::directoryExists((testDir / "non_existent").string()));
}

TEST_F(FileUtilsTest, ReadFromNonExistentFile) {
    std::string filePath = (testDir / "non_existent.txt").string();
    std::string content = FileUtils::readFromFile(filePath);
    EXPECT_TRUE(content.empty());
}
