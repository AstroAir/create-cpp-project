/**
 * @file test_file_analyzer.cpp
 * @brief Tests for FileAnalyzer class
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include "../src/core/file_analyzer.h"
#include <fstream>
#include <filesystem>

class FileAnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir_ = std::filesystem::temp_directory_path() / "file_analyzer_test";
        std::filesystem::create_directories(test_dir_);
        
        // Create test files
        create_test_files();
    }
    
    void TearDown() override {
        std::filesystem::remove_all(test_dir_);
    }
    
    void create_test_files() {
        // ASCII text file
        std::ofstream ascii_file(test_dir_ / "ascii.txt");
        ascii_file << "Hello World\nThis is ASCII text\n";
        
        // UTF-8 file with BOM
        std::ofstream utf8_file(test_dir_ / "utf8.txt", std::ios::binary);
        utf8_file << "\xEF\xBB\xBF" << "UTF-8 text with BOM\n";
        
        // Binary file
        std::ofstream binary_file(test_dir_ / "binary.dat", std::ios::binary);
        for (int i = 0; i < 256; ++i) {
            binary_file.put(static_cast<char>(i));
        }
    }
    
    std::filesystem::path test_dir_;
};

TEST_F(FileAnalyzerTest, DetectEncoding) {
    EXPECT_EQ(core::FileAnalyzer::detect_encoding(test_dir_ / "ascii.txt"), "ASCII");
    EXPECT_EQ(core::FileAnalyzer::detect_encoding(test_dir_ / "utf8.txt"), "UTF-8");
}

TEST_F(FileAnalyzerTest, DetectFileType) {
    EXPECT_EQ(core::FileAnalyzer::detect_file_type(test_dir_ / "ascii.txt"), "Text");
    EXPECT_EQ(core::FileAnalyzer::detect_file_type(test_dir_ / "binary.dat"), "Unknown");
}

TEST_F(FileAnalyzerTest, IsBinaryFile) {
    EXPECT_FALSE(core::FileAnalyzer::is_binary_file(test_dir_ / "ascii.txt"));
    EXPECT_TRUE(core::FileAnalyzer::is_binary_file(test_dir_ / "binary.dat"));
}

TEST_F(FileAnalyzerTest, GetFileSize) {
    auto size = core::FileAnalyzer::get_file_size(test_dir_ / "ascii.txt");
    EXPECT_GT(size, 0);
}
