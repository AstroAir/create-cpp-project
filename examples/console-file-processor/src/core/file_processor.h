/**
 * @file file_processor.h
 * @brief Core file processing functionality
 * 
 * This file contains the main FileProcessor class that handles all file
 * processing operations including counting, searching, and text processing.
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <regex>
#include <fstream>
#include <filesystem>

namespace core {

/**
 * @brief File statistics structure
 */
struct FileStats {
    std::size_t lines = 0;          ///< Number of lines
    std::size_t words = 0;          ///< Number of words
    std::size_t characters = 0;     ///< Number of characters
    std::size_t bytes = 0;          ///< Number of bytes
    std::string encoding = "unknown"; ///< Detected encoding
    std::string file_type = "unknown"; ///< Detected file type
    
    /**
     * @brief Add another FileStats to this one
     * @param other The FileStats to add
     * @return Reference to this FileStats
     */
    FileStats& operator+=(const FileStats& other);
    
    /**
     * @brief Convert to JSON string
     * @return JSON representation
     */
    std::string to_json() const;
    
    /**
     * @brief Convert to CSV string
     * @return CSV representation
     */
    std::string to_csv() const;
};

/**
 * @brief Search result structure
 */
struct SearchResult {
    std::string filename;           ///< File where match was found
    std::size_t line_number = 0;   ///< Line number (1-based)
    std::size_t column = 0;         ///< Column position (0-based)
    std::string line_content;       ///< Content of the matching line
    std::vector<std::string> context_before; ///< Lines before match
    std::vector<std::string> context_after;  ///< Lines after match
    std::string matched_text;       ///< The actual matched text
    
    /**
     * @brief Convert to JSON string
     * @return JSON representation
     */
    std::string to_json() const;
};

/**
 * @brief Configuration structure for FileProcessor
 */
struct FileProcessorConfig {
    std::string format = "text";           ///< Output format (text, json, csv, xml)
    std::string output_file;               ///< Output file path (empty = stdout)
    std::string include_patterns;          ///< Include file patterns
    std::string exclude_patterns;          ///< Exclude file patterns
    std::size_t context_lines = 0;         ///< Context lines for search
    bool case_sensitive = false;           ///< Case-sensitive search
    bool whole_words = false;              ///< Match whole words only
    bool line_numbers = false;             ///< Show line numbers
    bool batch_mode = false;               ///< Batch processing mode
    std::size_t parallel_threads = 0;      ///< Number of parallel threads (0 = auto)
    bool show_progress = false;            ///< Show progress bar
    bool recursive = false;                ///< Process directories recursively
    std::string encoding = "auto";         ///< Text encoding
    std::size_t memory_limit = 1024 * 1024 * 1024; ///< Memory limit in bytes (1GB)
};

/**
 * @brief Main file processor class
 * 
 * This class provides comprehensive file processing capabilities including
 * file analysis, pattern searching, text processing, and batch operations.
 */
class FileProcessor {
public:
    using Config = FileProcessorConfig;
    using ProgressCallback = std::function<void(std::size_t current, std::size_t total)>;
    
    /**
     * @brief Constructor with configuration
     * @param config Processing configuration
     */
    explicit FileProcessor(const Config& config = Config{});
    
    /**
     * @brief Destructor
     */
    ~FileProcessor() = default;
    
    // Disable copy constructor and assignment
    FileProcessor(const FileProcessor&) = delete;
    FileProcessor& operator=(const FileProcessor&) = delete;
    
    // Enable move constructor and assignment
    FileProcessor(FileProcessor&&) = default;
    FileProcessor& operator=(FileProcessor&&) = default;
    
    /**
     * @brief Process files with basic analysis
     * @param files List of file paths to process
     * @return True if successful, false otherwise
     */
    bool process_files(const std::vector<std::string>& files);
    
    /**
     * @brief Count statistics for files
     * @param files List of file paths to analyze
     * @return True if successful, false otherwise
     */
    bool count_files(const std::vector<std::string>& files);
    
    /**
     * @brief Search for patterns in files
     * @param files List of file paths to search
     * @param pattern Regular expression pattern to search for
     * @return True if successful, false otherwise
     */
    bool search_files(const std::vector<std::string>& files, const std::string& pattern);
    
    /**
     * @brief Replace text in files
     * @param files List of file paths to process
     * @param old_text Text to replace
     * @param new_text Replacement text
     * @return True if successful, false otherwise
     */
    bool replace_text(const std::vector<std::string>& files, 
                     const std::string& old_text, 
                     const std::string& new_text);
    
    /**
     * @brief Set progress callback for long operations
     * @param callback Function to call with progress updates
     */
    void set_progress_callback(ProgressCallback callback);
    
    /**
     * @brief Get current configuration
     * @return Current configuration
     */
    const Config& get_config() const { return config_; }
    
    /**
     * @brief Update configuration
     * @param config New configuration
     */
    void set_config(const Config& config) { config_ = config; }

private:
    Config config_;                         ///< Processing configuration
    ProgressCallback progress_callback_;    ///< Progress callback function
    std::unique_ptr<std::ofstream> output_stream_; ///< Output file stream
    
    /**
     * @brief Expand file list with recursive directory processing
     * @param files Input file list
     * @return Expanded file list
     */
    std::vector<std::string> expand_file_list(const std::vector<std::string>& files);
    
    /**
     * @brief Filter files based on include/exclude patterns
     * @param files Input file list
     * @return Filtered file list
     */
    std::vector<std::string> filter_files(const std::vector<std::string>& files);
    
    /**
     * @brief Check if file matches pattern
     * @param filename File name to check
     * @param patterns Comma-separated patterns
     * @return True if matches any pattern
     */
    bool matches_patterns(const std::string& filename, const std::string& patterns);
    
    /**
     * @brief Analyze a single file
     * @param filename Path to file
     * @return File statistics
     */
    FileStats analyze_file(const std::string& filename);
    
    /**
     * @brief Search patterns in a single file
     * @param filename Path to file
     * @param pattern Regular expression pattern
     * @return Vector of search results
     */
    std::vector<SearchResult> search_file(const std::string& filename, const std::regex& pattern);
    
    /**
     * @brief Detect file encoding
     * @param filename Path to file
     * @return Detected encoding string
     */
    std::string detect_encoding(const std::string& filename);
    
    /**
     * @brief Detect file type
     * @param filename Path to file
     * @return Detected file type string
     */
    std::string detect_file_type(const std::string& filename);
    
    /**
     * @brief Get output stream (file or stdout)
     * @return Reference to output stream
     */
    std::ostream& get_output_stream();
    
    /**
     * @brief Format and output file statistics
     * @param stats Vector of file statistics
     */
    void output_stats(const std::vector<std::pair<std::string, FileStats>>& stats);
    
    /**
     * @brief Format and output search results
     * @param results Vector of search results
     */
    void output_search_results(const std::vector<SearchResult>& results);
    
    /**
     * @brief Update progress if callback is set
     * @param current Current progress value
     * @param total Total progress value
     */
    void update_progress(std::size_t current, std::size_t total);
    
    /**
     * @brief Check if file is too large for processing
     * @param filename Path to file
     * @return True if file exceeds memory limit
     */
    bool is_file_too_large(const std::string& filename);
    
    /**
     * @brief Process large file in chunks
     * @param filename Path to large file
     * @param processor Function to process each chunk
     * @return True if successful
     */
    bool process_large_file(const std::string& filename, 
                           std::function<bool(const std::string&)> processor);
    
    /**
     * @brief Create regex from pattern with configuration
     * @param pattern Input pattern string
     * @return Compiled regex object
     */
    std::regex create_regex(const std::string& pattern);
    
    /**
     * @brief Log processing information
     * @param message Log message
     */
    void log_info(const std::string& message);
    
    /**
     * @brief Log warning message
     * @param message Warning message
     */
    void log_warning(const std::string& message);
    
    /**
     * @brief Log error message
     * @param message Error message
     */
    void log_error(const std::string& message);
};

} // namespace core
