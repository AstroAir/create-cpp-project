/**
 * @file test_file_processor.cpp
 * @brief Comprehensive tests for the FileProcessor class
 * 
 * This file demonstrates testing patterns and serves as usage examples
 * for the FileProcessor functionality.
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <string>

#include "../src/core/file_processor.h"
#include "../src/utils/logger.h"

namespace fs = std::filesystem;

/**
 * @brief Test fixture for FileProcessor tests
 * 
 * Sets up test files and directories for comprehensive testing
 */
class FileProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logger for tests
        utils::Logger::init();
        utils::Logger::set_level(utils::LogLevel::Error); // Reduce noise in tests
        
        // Create test directory
        test_dir_ = fs::temp_directory_path() / "file_processor_test";
        fs::create_directories(test_dir_);
        
        // Create test files
        create_test_files();
    }
    
    void TearDown() override {
        // Clean up test directory
        if (fs::exists(test_dir_)) {
            fs::remove_all(test_dir_);
        }
    }
    
    void create_test_files() {
        // Simple text file
        auto simple_file = test_dir_ / "simple.txt";
        std::ofstream(simple_file) << "Hello world\nThis is a test\nAnother line\n";
        
        // Empty file
        auto empty_file = test_dir_ / "empty.txt";
        std::ofstream(empty_file);
        
        // Large file with patterns
        auto pattern_file = test_dir_ / "patterns.cpp";
        std::ofstream pattern_stream(pattern_file);
        pattern_stream << "#include <iostream>\n";
        pattern_stream << "#include <string>\n";
        pattern_stream << "\n";
        pattern_stream << "// TODO: Implement this function\n";
        pattern_stream << "int main() {\n";
        pattern_stream << "    std::cout << \"Hello, World!\" << std::endl;\n";
        pattern_stream << "    // FIXME: Add error handling\n";
        pattern_stream << "    return 0;\n";
        pattern_stream << "}\n";
        
        // Binary-like file
        auto binary_file = test_dir_ / "binary.dat";
        std::ofstream binary_stream(binary_file, std::ios::binary);
        for (int i = 0; i < 256; ++i) {
            binary_stream.put(static_cast<char>(i));
        }
        
        // Subdirectory with files
        auto sub_dir = test_dir_ / "subdir";
        fs::create_directory(sub_dir);
        std::ofstream(sub_dir / "nested.txt") << "Nested file content\nLine 2\n";
        std::ofstream(sub_dir / "another.md") << "# Markdown\nSome content\n";
        
        // Unicode file
        auto unicode_file = test_dir_ / "unicode.txt";
        std::ofstream unicode_stream(unicode_file);
        unicode_stream << "English text\n";
        unicode_stream << "Español\n";
        unicode_stream << "Français\n";
        unicode_stream << "日本語\n";
    }
    
    fs::path test_dir_;
};

/**
 * @brief Test basic file analysis functionality
 */
TEST_F(FileProcessorTest, BasicFileAnalysis) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "simple.txt").string()
    };
    
    EXPECT_TRUE(processor.process_files(files));
}

/**
 * @brief Test file counting functionality
 */
TEST_F(FileProcessorTest, FileCountingBasic) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "simple.txt").string()
    };
    
    EXPECT_TRUE(processor.count_files(files));
}

/**
 * @brief Test file counting with multiple files
 */
TEST_F(FileProcessorTest, FileCountingMultiple) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "simple.txt").string(),
        (test_dir_ / "patterns.cpp").string(),
        (test_dir_ / "unicode.txt").string()
    };
    
    EXPECT_TRUE(processor.count_files(files));
}

/**
 * @brief Test pattern searching functionality
 */
TEST_F(FileProcessorTest, PatternSearchBasic) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "patterns.cpp").string()
    };
    
    EXPECT_TRUE(processor.search_files(files, "TODO"));
    EXPECT_TRUE(processor.search_files(files, "FIXME"));
    EXPECT_TRUE(processor.search_files(files, "include"));
}

/**
 * @brief Test pattern searching with regex
 */
TEST_F(FileProcessorTest, PatternSearchRegex) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "patterns.cpp").string()
    };
    
    // Test regex patterns
    EXPECT_TRUE(processor.search_files(files, "TODO|FIXME"));
    EXPECT_TRUE(processor.search_files(files, "std::\\w+"));
    EXPECT_TRUE(processor.search_files(files, "#include\\s+<\\w+>"));
}

/**
 * @brief Test case-sensitive search
 */
TEST_F(FileProcessorTest, CaseSensitiveSearch) {
    core::FileProcessor::Config config;
    config.case_sensitive = true;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "patterns.cpp").string()
    };
    
    EXPECT_TRUE(processor.search_files(files, "TODO"));
    EXPECT_TRUE(processor.search_files(files, "todo")); // Should not match
}

/**
 * @brief Test search with context lines
 */
TEST_F(FileProcessorTest, SearchWithContext) {
    core::FileProcessor::Config config;
    config.context_lines = 2;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "patterns.cpp").string()
    };
    
    EXPECT_TRUE(processor.search_files(files, "TODO"));
}

/**
 * @brief Test recursive directory processing
 */
TEST_F(FileProcessorTest, RecursiveProcessing) {
    core::FileProcessor::Config config;
    config.recursive = true;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        test_dir_.string()
    };
    
    EXPECT_TRUE(processor.process_files(files));
}

/**
 * @brief Test file filtering with include patterns
 */
TEST_F(FileProcessorTest, IncludePatterns) {
    core::FileProcessor::Config config;
    config.include_patterns = "*.cpp,*.h";
    config.recursive = true;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        test_dir_.string()
    };
    
    EXPECT_TRUE(processor.process_files(files));
}

/**
 * @brief Test file filtering with exclude patterns
 */
TEST_F(FileProcessorTest, ExcludePatterns) {
    core::FileProcessor::Config config;
    config.exclude_patterns = "*.dat,*.bin";
    config.recursive = true;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        test_dir_.string()
    };
    
    EXPECT_TRUE(processor.process_files(files));
}

/**
 * @brief Test JSON output format
 */
TEST_F(FileProcessorTest, JsonOutput) {
    core::FileProcessor::Config config;
    config.format = "json";
    auto output_file = test_dir_ / "output.json";
    config.output_file = output_file.string();
    
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "simple.txt").string()
    };
    
    EXPECT_TRUE(processor.count_files(files));
    EXPECT_TRUE(fs::exists(output_file));
    
    // Verify JSON content
    std::ifstream json_file(output_file);
    std::string content((std::istreambuf_iterator<char>(json_file)),
                       std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("\"files\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"lines\"") != std::string::npos);
}

/**
 * @brief Test CSV output format
 */
TEST_F(FileProcessorTest, CsvOutput) {
    core::FileProcessor::Config config;
    config.format = "csv";
    auto output_file = test_dir_ / "output.csv";
    config.output_file = output_file.string();
    
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "simple.txt").string(),
        (test_dir_ / "patterns.cpp").string()
    };
    
    EXPECT_TRUE(processor.count_files(files));
    EXPECT_TRUE(fs::exists(output_file));
    
    // Verify CSV content
    std::ifstream csv_file(output_file);
    std::string header;
    std::getline(csv_file, header);
    EXPECT_TRUE(header.find("filename") != std::string::npos);
    EXPECT_TRUE(header.find("lines") != std::string::npos);
}

/**
 * @brief Test error handling for non-existent files
 */
TEST_F(FileProcessorTest, NonExistentFiles) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "nonexistent.txt").string()
    };
    
    // Should handle gracefully and return true (no files processed)
    EXPECT_TRUE(processor.process_files(files));
}

/**
 * @brief Test empty file handling
 */
TEST_F(FileProcessorTest, EmptyFiles) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::vector<std::string> files = {
        (test_dir_ / "empty.txt").string()
    };
    
    EXPECT_TRUE(processor.count_files(files));
}

/**
 * @brief Test progress callback functionality
 */
TEST_F(FileProcessorTest, ProgressCallback) {
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    std::size_t progress_calls = 0;
    std::size_t last_current = 0;
    std::size_t last_total = 0;
    
    processor.set_progress_callback([&](std::size_t current, std::size_t total) {
        progress_calls++;
        last_current = current;
        last_total = total;
    });
    
    std::vector<std::string> files = {
        (test_dir_ / "simple.txt").string(),
        (test_dir_ / "patterns.cpp").string(),
        (test_dir_ / "unicode.txt").string()
    };
    
    EXPECT_TRUE(processor.process_files(files));
    EXPECT_GT(progress_calls, 0);
    EXPECT_EQ(last_current, last_total); // Should end with 100% progress
}

/**
 * @brief Performance test for large number of files
 */
TEST_F(FileProcessorTest, PerformanceTest) {
    // Create many small files
    auto perf_dir = test_dir_ / "performance";
    fs::create_directory(perf_dir);
    
    std::vector<std::string> files;
    for (int i = 0; i < 100; ++i) {
        auto file_path = perf_dir / ("file_" + std::to_string(i) + ".txt");
        std::ofstream(file_path) << "Content for file " << i << "\nLine 2\nLine 3\n";
        files.push_back(file_path.string());
    }
    
    core::FileProcessor::Config config;
    core::FileProcessor processor(config);
    
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_TRUE(processor.count_files(files));
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should process 100 small files in reasonable time (less than 1 second)
    EXPECT_LT(duration.count(), 1000);
}

/**
 * @brief Integration test combining multiple features
 */
TEST_F(FileProcessorTest, IntegrationTest) {
    core::FileProcessor::Config config;
    config.format = "json";
    config.recursive = true;
    config.include_patterns = "*.txt,*.cpp,*.md";
    config.context_lines = 1;
    auto output_file = test_dir_ / "integration_output.json";
    config.output_file = output_file.string();
    
    core::FileProcessor processor(config);
    
    // Test search with all features enabled
    std::vector<std::string> files = {
        test_dir_.string()
    };
    
    EXPECT_TRUE(processor.search_files(files, "content|TODO"));
    EXPECT_TRUE(fs::exists(output_file));
    
    // Verify output contains expected structure
    std::ifstream json_file(output_file);
    std::string content((std::istreambuf_iterator<char>(json_file)),
                       std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("\"matches\"") != std::string::npos);
}
