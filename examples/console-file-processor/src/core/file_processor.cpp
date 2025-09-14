/**
 * @file file_processor.cpp
 * @brief Implementation of the FileProcessor class
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include "file_processor.h"
#include "file_analyzer.h"
#include "text_processor.h"
#include "../utils/logger.h"
#include "../utils/core/file_utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <future>
#include <iomanip>
#include <cctype>

namespace fs = std::filesystem;

namespace core {

// FileStats implementation
FileStats& FileStats::operator+=(const FileStats& other) {
    lines += other.lines;
    words += other.words;
    characters += other.characters;
    bytes += other.bytes;
    return *this;
}

std::string FileStats::to_json() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"lines\": " << lines << ",\n";
    oss << "  \"words\": " << words << ",\n";
    oss << "  \"characters\": " << characters << ",\n";
    oss << "  \"bytes\": " << bytes << ",\n";
    oss << "  \"encoding\": \"" << encoding << "\",\n";
    oss << "  \"file_type\": \"" << file_type << "\"\n";
    oss << "}";
    return oss.str();
}

std::string FileStats::to_csv() const {
    std::ostringstream oss;
    oss << lines << "," << words << "," << characters << "," << bytes 
        << ",\"" << encoding << "\",\"" << file_type << "\"";
    return oss.str();
}

// SearchResult implementation
std::string SearchResult::to_json() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"filename\": \"" << filename << "\",\n";
    oss << "  \"line_number\": " << line_number << ",\n";
    oss << "  \"column\": " << column << ",\n";
    oss << "  \"line_content\": \"" << line_content << "\",\n";
    oss << "  \"matched_text\": \"" << matched_text << "\"\n";
    oss << "}";
    return oss.str();
}

// FileProcessor implementation
FileProcessor::FileProcessor(const Config& config) : config_(config) {
    if (!config_.output_file.empty()) {
        output_stream_ = std::make_unique<std::ofstream>(config_.output_file);
        if (!output_stream_->is_open()) {
            throw std::runtime_error("Cannot open output file: " + config_.output_file);
        }
    }
}

bool FileProcessor::process_files(const std::vector<std::string>& files) {
    try {
        auto expanded_files = expand_file_list(files);
        auto filtered_files = filter_files(expanded_files);
        
        if (filtered_files.empty()) {
            log_warning("No files to process after filtering");
            return true;
        }
        
        log_info("Processing " + std::to_string(filtered_files.size()) + " files");
        
        std::vector<std::pair<std::string, FileStats>> results;
        results.reserve(filtered_files.size());
        
        for (std::size_t i = 0; i < filtered_files.size(); ++i) {
            const auto& file = filtered_files[i];
            update_progress(i, filtered_files.size());
            
            try {
                auto stats = analyze_file(file);
                results.emplace_back(file, std::move(stats));
            } catch (const std::exception& e) {
                log_error("Error processing file " + file + ": " + e.what());
                continue;
            }
        }
        
        update_progress(filtered_files.size(), filtered_files.size());
        output_stats(results);
        
        return true;
    } catch (const std::exception& e) {
        log_error("Error in process_files: " + std::string(e.what()));
        return false;
    }
}

bool FileProcessor::count_files(const std::vector<std::string>& files) {
    try {
        auto expanded_files = expand_file_list(files);
        auto filtered_files = filter_files(expanded_files);
        
        if (filtered_files.empty()) {
            log_warning("No files to count after filtering");
            return true;
        }
        
        log_info("Counting statistics for " + std::to_string(filtered_files.size()) + " files");
        
        std::vector<std::pair<std::string, FileStats>> results;
        results.reserve(filtered_files.size());
        
        FileStats total_stats;
        
        for (std::size_t i = 0; i < filtered_files.size(); ++i) {
            const auto& file = filtered_files[i];
            update_progress(i, filtered_files.size());
            
            try {
                auto stats = analyze_file(file);
                total_stats += stats;
                results.emplace_back(file, std::move(stats));
            } catch (const std::exception& e) {
                log_error("Error counting file " + file + ": " + e.what());
                continue;
            }
        }
        
        update_progress(filtered_files.size(), filtered_files.size());
        
        // Add total to results
        results.emplace_back("TOTAL", std::move(total_stats));
        output_stats(results);
        
        return true;
    } catch (const std::exception& e) {
        log_error("Error in count_files: " + std::string(e.what()));
        return false;
    }
}

bool FileProcessor::search_files(const std::vector<std::string>& files, const std::string& pattern) {
    try {
        auto expanded_files = expand_file_list(files);
        auto filtered_files = filter_files(expanded_files);
        
        if (filtered_files.empty()) {
            log_warning("No files to search after filtering");
            return true;
        }
        
        log_info("Searching pattern '" + pattern + "' in " + std::to_string(filtered_files.size()) + " files");
        
        auto regex_pattern = create_regex(pattern);
        std::vector<SearchResult> all_results;
        
        for (std::size_t i = 0; i < filtered_files.size(); ++i) {
            const auto& file = filtered_files[i];
            update_progress(i, filtered_files.size());
            
            try {
                auto results = search_file(file, regex_pattern);
                all_results.insert(all_results.end(), results.begin(), results.end());
            } catch (const std::exception& e) {
                log_error("Error searching file " + file + ": " + e.what());
                continue;
            }
        }
        
        update_progress(filtered_files.size(), filtered_files.size());
        output_search_results(all_results);
        
        return true;
    } catch (const std::exception& e) {
        log_error("Error in search_files: " + std::string(e.what()));
        return false;
    }
}

void FileProcessor::set_progress_callback(ProgressCallback callback) {
    progress_callback_ = std::move(callback);
}

std::vector<std::string> FileProcessor::expand_file_list(const std::vector<std::string>& files) {
    std::vector<std::string> expanded;
    
    for (const auto& file_path : files) {
        fs::path path(file_path);
        
        if (fs::is_regular_file(path)) {
            expanded.push_back(file_path);
        } else if (fs::is_directory(path) && config_.recursive) {
            try {
                for (const auto& entry : fs::recursive_directory_iterator(path)) {
                    if (entry.is_regular_file()) {
                        expanded.push_back(entry.path().string());
                    }
                }
            } catch (const fs::filesystem_error& e) {
                log_warning("Error accessing directory " + file_path + ": " + e.what());
            }
        }
    }
    
    return expanded;
}

std::vector<std::string> FileProcessor::filter_files(const std::vector<std::string>& files) {
    std::vector<std::string> filtered;
    
    for (const auto& file : files) {
        bool include = true;
        
        // Check include patterns
        if (!config_.include_patterns.empty()) {
            include = matches_patterns(file, config_.include_patterns);
        }
        
        // Check exclude patterns
        if (include && !config_.exclude_patterns.empty()) {
            include = !matches_patterns(file, config_.exclude_patterns);
        }
        
        if (include) {
            filtered.push_back(file);
        }
    }
    
    return filtered;
}

bool FileProcessor::matches_patterns(const std::string& filename, const std::string& patterns) {
    if (patterns.empty()) return false;
    
    std::istringstream iss(patterns);
    std::string pattern;
    
    while (std::getline(iss, pattern, ',')) {
        // Trim whitespace
        pattern.erase(0, pattern.find_first_not_of(" \t"));
        pattern.erase(pattern.find_last_not_of(" \t") + 1);
        
        // Simple wildcard matching (convert * to .* for regex)
        std::string regex_pattern = pattern;
        std::replace(regex_pattern.begin(), regex_pattern.end(), '*', '.');
        regex_pattern = ".*" + regex_pattern + ".*";
        
        try {
            std::regex re(regex_pattern, std::regex_constants::icase);
            if (std::regex_match(filename, re)) {
                return true;
            }
        } catch (const std::regex_error&) {
            // If regex fails, try simple string matching
            if (filename.find(pattern) != std::string::npos) {
                return true;
            }
        }
    }
    
    return false;
}

FileStats FileProcessor::analyze_file(const std::string& filename) {
    FileStats stats;
    
    if (is_file_too_large(filename)) {
        log_warning("File too large, processing in chunks: " + filename);
        // For large files, we'd implement chunked processing
        // For now, just return basic stats
        stats.bytes = fs::file_size(filename);
        stats.encoding = detect_encoding(filename);
        stats.file_type = detect_file_type(filename);
        return stats;
    }
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    stats.encoding = detect_encoding(filename);
    stats.file_type = detect_file_type(filename);
    
    std::string line;
    while (std::getline(file, line)) {
        stats.lines++;
        stats.characters += line.length() + 1; // +1 for newline
        
        // Count words
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            stats.words++;
        }
    }
    
    stats.bytes = file.tellg();
    return stats;
}

std::vector<SearchResult> FileProcessor::search_file(const std::string& filename, const std::regex& pattern) {
    std::vector<SearchResult> results;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::smatch match;
        if (std::regex_search(lines[i], match, pattern)) {
            SearchResult result;
            result.filename = filename;
            result.line_number = i + 1;
            result.column = match.position();
            result.line_content = lines[i];
            result.matched_text = match.str();
            
            // Add context lines
            std::size_t context = config_.context_lines;
            for (std::size_t j = (i >= context) ? i - context : 0; j < i; ++j) {
                result.context_before.push_back(lines[j]);
            }
            for (std::size_t j = i + 1; j < std::min(i + 1 + context, lines.size()); ++j) {
                result.context_after.push_back(lines[j]);
            }
            
            results.push_back(std::move(result));
        }
    }
    
    return results;
}

std::string FileProcessor::detect_encoding(const std::string& filename) {
    // Simple encoding detection - in a real implementation,
    // you'd use a proper encoding detection library
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return "unknown";
    
    // Check for BOM
    char bom[3];
    file.read(bom, 3);
    
    if (bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF') {
        return "UTF-8";
    } else if (bom[0] == '\xFF' && bom[1] == '\xFE') {
        return "UTF-16LE";
    } else if (bom[0] == '\xFE' && bom[1] == '\xFF') {
        return "UTF-16BE";
    }
    
    return "ASCII"; // Default assumption
}

std::string FileProcessor::detect_file_type(const std::string& filename) {
    fs::path path(filename);
    auto extension = path.extension().string();
    
    if (extension.empty()) return "text";
    
    // Convert to lowercase
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".cpp" || extension == ".cc" || extension == ".cxx") return "C++";
    if (extension == ".h" || extension == ".hpp" || extension == ".hxx") return "C++ Header";
    if (extension == ".c") return "C";
    if (extension == ".py") return "Python";
    if (extension == ".js") return "JavaScript";
    if (extension == ".txt") return "Text";
    if (extension == ".md") return "Markdown";
    if (extension == ".json") return "JSON";
    if (extension == ".xml") return "XML";
    if (extension == ".html" || extension == ".htm") return "HTML";
    
    return "Unknown";
}

std::ostream& FileProcessor::get_output_stream() {
    return output_stream_ ? *output_stream_ : std::cout;
}

void FileProcessor::output_stats(const std::vector<std::pair<std::string, FileStats>>& stats) {
    auto& out = get_output_stream();
    
    if (config_.format == "json") {
        out << "{\n  \"files\": [\n";
        for (std::size_t i = 0; i < stats.size(); ++i) {
            out << "    {\n";
            out << "      \"filename\": \"" << stats[i].first << "\",\n";
            out << "      \"stats\": " << stats[i].second.to_json() << "\n";
            out << "    }";
            if (i < stats.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ]\n}\n";
    } else if (config_.format == "csv") {
        out << "filename,lines,words,characters,bytes,encoding,file_type\n";
        for (const auto& [filename, file_stats] : stats) {
            out << "\"" << filename << "\"," << file_stats.to_csv() << "\n";
        }
    } else {
        // Text format
        out << std::left << std::setw(30) << "File" 
            << std::setw(10) << "Lines" 
            << std::setw(10) << "Words" 
            << std::setw(12) << "Characters"
            << std::setw(10) << "Bytes"
            << std::setw(12) << "Encoding"
            << "Type\n";
        out << std::string(84, '-') << "\n";
        
        for (const auto& [filename, file_stats] : stats) {
            out << std::left << std::setw(30) << filename
                << std::setw(10) << file_stats.lines
                << std::setw(10) << file_stats.words
                << std::setw(12) << file_stats.characters
                << std::setw(10) << file_stats.bytes
                << std::setw(12) << file_stats.encoding
                << file_stats.file_type << "\n";
        }
    }
}

void FileProcessor::output_search_results(const std::vector<SearchResult>& results) {
    auto& out = get_output_stream();
    
    if (config_.format == "json") {
        out << "{\n  \"matches\": [\n";
        for (std::size_t i = 0; i < results.size(); ++i) {
            out << "    " << results[i].to_json();
            if (i < results.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ]\n}\n";
    } else {
        // Text format
        std::string current_file;
        for (const auto& result : results) {
            if (result.filename != current_file) {
                if (!current_file.empty()) out << "\n";
                out << result.filename << ":\n";
                current_file = result.filename;
            }
            
            if (config_.line_numbers) {
                out << "  " << result.line_number << ": ";
            } else {
                out << "  ";
            }
            out << result.line_content << "\n";
        }
    }
}

void FileProcessor::update_progress(std::size_t current, std::size_t total) {
    if (progress_callback_) {
        progress_callback_(current, total);
    }
}

bool FileProcessor::is_file_too_large(const std::string& filename) {
    try {
        auto size = fs::file_size(filename);
        return size > config_.memory_limit;
    } catch (const fs::filesystem_error&) {
        return false;
    }
}

std::regex FileProcessor::create_regex(const std::string& pattern) {
    std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;
    
    if (!config_.case_sensitive) {
        flags |= std::regex_constants::icase;
    }
    
    std::string regex_pattern = pattern;
    if (config_.whole_words) {
        regex_pattern = "\\b" + pattern + "\\b";
    }
    
    return std::regex(regex_pattern, flags);
}

void FileProcessor::log_info(const std::string& message) {
    utils::Logger::info(message);
}

void FileProcessor::log_warning(const std::string& message) {
    utils::Logger::warn(message);
}

void FileProcessor::log_error(const std::string& message) {
    utils::Logger::error(message);
}

} // namespace core
