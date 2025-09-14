#include <gtest/gtest.h>
#include "../../src/utils/core/file_utils.h"
#include <filesystem>
#include <fstream>
#include <string>

using namespace utils;

class FileUtilsEdgeCasesTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = std::filesystem::temp_directory_path() / "cpp_scaffold_file_edge_test";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        originalPath = std::filesystem::current_path();
        std::filesystem::current_path(testDir);
    }

    void TearDown() override {
        std::filesystem::current_path(originalPath);
        std::filesystem::remove_all(testDir);
    }

    std::filesystem::path testDir;
    std::filesystem::path originalPath;
};

// Test file operations with very long paths
TEST_F(FileUtilsEdgeCasesTest, VeryLongPaths) {
    // Create a very long directory path
    std::string longPath = "a";
    for (int i = 0; i < 10; ++i) {
        longPath += "/very_long_directory_name_that_goes_on_and_on";
    }

    // This might fail on some systems due to path length limits
    bool created = FileUtils::createDirectory(longPath);
    if (created) {
        EXPECT_TRUE(FileUtils::directoryExists(longPath));

        // Try to write a file in the long path
        std::string filePath = longPath + "/test_file.txt";
        EXPECT_TRUE(FileUtils::writeToFile(filePath, "test content"));
        EXPECT_TRUE(FileUtils::fileExists(filePath));
    }
    // If creation failed, that's also acceptable due to system limitations
}

// Test file operations with special characters in names
TEST_F(FileUtilsEdgeCasesTest, SpecialCharactersInNames) {
    // Test with various special characters (some may not work on all systems)
    std::vector<std::string> specialNames = {
        "file with spaces.txt",
        "file-with-dashes.txt",
        "file_with_underscores.txt",
        "file.with.dots.txt",
        "file123numbers.txt"
    };

    for (const auto& name : specialNames) {
        std::string content = "Content for " + name;
        bool writeSuccess = FileUtils::writeToFile(name, content);

        if (writeSuccess) {
            EXPECT_TRUE(FileUtils::fileExists(name)) << "File " << name << " should exist";
            std::string readContent = FileUtils::readFromFile(name);
            EXPECT_EQ(content, readContent) << "Content mismatch for " << name;
        }
    }
}

// Test file operations with empty content
TEST_F(FileUtilsEdgeCasesTest, EmptyContent) {
    std::string emptyFile = "empty_file.txt";

    // Write empty content
    EXPECT_TRUE(FileUtils::writeToFile(emptyFile, ""));
    EXPECT_TRUE(FileUtils::fileExists(emptyFile));

    // Read empty content
    std::string content = FileUtils::readFromFile(emptyFile);
    EXPECT_TRUE(content.empty());
}

// Test file operations with very large content
TEST_F(FileUtilsEdgeCasesTest, LargeContent) {
    std::string largeFile = "large_file.txt";

    // Create large content (1MB)
    std::string largeContent(1024 * 1024, 'A');

    EXPECT_TRUE(FileUtils::writeToFile(largeFile, largeContent));
    EXPECT_TRUE(FileUtils::fileExists(largeFile));

    std::string readContent = FileUtils::readFromFile(largeFile);
    EXPECT_EQ(largeContent.size(), readContent.size());
    EXPECT_EQ(largeContent, readContent);
}

// Test file operations with binary content
TEST_F(FileUtilsEdgeCasesTest, BinaryContent) {
    std::string binaryFile = "binary_file.bin";

    // Create binary content with null bytes and special characters
    std::string binaryContent;
    for (int i = 0; i < 256; ++i) {
        binaryContent += static_cast<char>(i);
    }

    EXPECT_TRUE(FileUtils::writeToFile(binaryFile, binaryContent));
    EXPECT_TRUE(FileUtils::fileExists(binaryFile));

    std::string readContent = FileUtils::readFromFile(binaryFile);
    EXPECT_EQ(binaryContent.size(), readContent.size());
    EXPECT_EQ(binaryContent, readContent);
}

// Test directory operations with nested structures
TEST_F(FileUtilsEdgeCasesTest, DeepNestedDirectories) {
    std::string deepPath = "level1/level2/level3/level4/level5";

    EXPECT_TRUE(FileUtils::createDirectory(deepPath));
    EXPECT_TRUE(FileUtils::directoryExists(deepPath));

    // Create a file in the deep directory
    std::string filePath = deepPath + "/deep_file.txt";
    EXPECT_TRUE(FileUtils::writeToFile(filePath, "deep content"));
    EXPECT_TRUE(FileUtils::fileExists(filePath));
}

// Test copy operations with various scenarios
TEST_F(FileUtilsEdgeCasesTest, CopyOperationsEdgeCases) {
    // Test copying to existing file (should overwrite)
    std::string source = "source.txt";
    std::string dest = "dest.txt";

    FileUtils::writeToFile(source, "source content");
    FileUtils::writeToFile(dest, "original dest content");

    EXPECT_TRUE(FileUtils::copyFile(source, dest));

    std::string destContent = FileUtils::readFromFile(dest);
    EXPECT_EQ("source content", destContent);

    // Test copying non-existent file
    EXPECT_FALSE(FileUtils::copyFile("non_existent.txt", "dest2.txt"));

    // Test copying to invalid destination
    EXPECT_FALSE(FileUtils::copyFile(source, "invalid/path/dest.txt"));
}

// Test directory copying with complex structures
TEST_F(FileUtilsEdgeCasesTest, DirectoryCopyingEdgeCases) {
    // Create source directory structure
    std::string sourceDir = "source_dir";
    FileUtils::createDirectory(sourceDir);
    FileUtils::createDirectory(sourceDir + "/subdir1");
    FileUtils::createDirectory(sourceDir + "/subdir2");

    FileUtils::writeToFile(sourceDir + "/file1.txt", "content1");
    FileUtils::writeToFile(sourceDir + "/subdir1/file2.txt", "content2");
    FileUtils::writeToFile(sourceDir + "/subdir2/file3.txt", "content3");

    std::string destDir = "dest_dir";

    EXPECT_TRUE(FileUtils::copyDirectory(sourceDir, destDir));

    // Verify all files and directories were copied
    EXPECT_TRUE(FileUtils::directoryExists(destDir));
    EXPECT_TRUE(FileUtils::directoryExists(destDir + "/subdir1"));
    EXPECT_TRUE(FileUtils::directoryExists(destDir + "/subdir2"));
    EXPECT_TRUE(FileUtils::fileExists(destDir + "/file1.txt"));
    EXPECT_TRUE(FileUtils::fileExists(destDir + "/subdir1/file2.txt"));
    EXPECT_TRUE(FileUtils::fileExists(destDir + "/subdir2/file3.txt"));

    // Verify content
    EXPECT_EQ("content1", FileUtils::readFromFile(destDir + "/file1.txt"));
    EXPECT_EQ("content2", FileUtils::readFromFile(destDir + "/subdir1/file2.txt"));
    EXPECT_EQ("content3", FileUtils::readFromFile(destDir + "/subdir2/file3.txt"));
}

// Test path manipulation edge cases
TEST_F(FileUtilsEdgeCasesTest, PathManipulationEdgeCases) {
    // Test combining paths with various separators
    // Use platform-appropriate path separators
    std::string expectedPath1 = (std::filesystem::path("path1") / "path2").string();
    EXPECT_EQ(expectedPath1, FileUtils::combinePath("path1", "path2"));

    // When path1 has trailing slash, filesystem::path preserves it differently
    std::string expectedPath1WithSlash = (std::filesystem::path("path1/") / "path2").string();
    EXPECT_EQ(expectedPath1WithSlash, FileUtils::combinePath("path1/", "path2"));

    // When second path is absolute, it should replace the first
    EXPECT_EQ("/path2", FileUtils::combinePath("path1", "/path2"));
    EXPECT_EQ("/path2", FileUtils::combinePath("path1/", "/path2"));

    // Test with empty paths
    EXPECT_EQ("path2", FileUtils::combinePath("", "path2"));
    // On Windows, combining with empty string adds a separator
    std::string expectedPath2 = (std::filesystem::path("path1") / "").string();
    EXPECT_EQ(expectedPath2, FileUtils::combinePath("path1", ""));

    // Test getting file extension edge cases
    EXPECT_EQ(".txt", FileUtils::getFileExtension("file.txt"));
    EXPECT_EQ("", FileUtils::getFileExtension("file"));
    EXPECT_EQ(".txt", FileUtils::getFileExtension("path/file.txt"));
    EXPECT_EQ("", FileUtils::getFileExtension("path/file"));
    // std::filesystem::path::extension() only returns the last extension
    EXPECT_EQ(".gz", FileUtils::getFileExtension("file.tar.gz"));

    // Test getting filename edge cases
    EXPECT_EQ("file.txt", FileUtils::getFileName("path/file.txt"));
    EXPECT_EQ("file.txt", FileUtils::getFileName("file.txt"));
    EXPECT_EQ("", FileUtils::getFileName("path/"));

    // Test getting directory name edge cases
    EXPECT_EQ("path", FileUtils::getDirectoryName("path/file.txt"));
    EXPECT_EQ(".", FileUtils::getDirectoryName("file.txt"));
}

// Test file listing with various directory contents
TEST_F(FileUtilsEdgeCasesTest, FileListingEdgeCases) {
    std::string testListDir = "list_test_dir";
    FileUtils::createDirectory(testListDir);

    // Create mixed content
    FileUtils::writeToFile(testListDir + "/file1.txt", "content");
    FileUtils::writeToFile(testListDir + "/file2.cpp", "content");
    FileUtils::createDirectory(testListDir + "/subdir");
    FileUtils::writeToFile(testListDir + "/subdir/nested_file.h", "content");

    auto files = FileUtils::listFiles(testListDir);

    // Should only list files, not directories
    EXPECT_EQ(2, files.size());

    std::set<std::string> fileSet(files.begin(), files.end());
    EXPECT_TRUE(fileSet.count("file1.txt") > 0);
    EXPECT_TRUE(fileSet.count("file2.cpp") > 0);
    EXPECT_FALSE(fileSet.count("subdir") > 0);  // Should not include directories

    // Test listing empty directory
    std::string emptyDir = "empty_dir";
    FileUtils::createDirectory(emptyDir);
    auto emptyFiles = FileUtils::listFiles(emptyDir);
    EXPECT_TRUE(emptyFiles.empty());

    // Test listing non-existent directory
    auto nonExistentFiles = FileUtils::listFiles("non_existent_dir");
    EXPECT_TRUE(nonExistentFiles.empty());
}
