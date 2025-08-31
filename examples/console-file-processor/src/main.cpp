/**
 * @file main.cpp
 * @brief File Processor - Main application entry point
 * 
 * A comprehensive file processing utility demonstrating modern C++ practices,
 * command-line argument parsing, file I/O, and error handling.
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <filesystem>

#include "cli/argument_parser.h"
#include "core/file_processor.h"
#include "utils/logger.h"
#include "file-processor/version.h"

namespace fs = std::filesystem;

/**
 * @brief Print application banner and version information
 */
void print_banner() {
    std::cout << "File Processor v" << FILE_PROCESSOR_VERSION << "\n";
    std::cout << "A modern C++ file processing utility\n";
    std::cout << "Generated with CPP-Scaffold\n\n";
}

/**
 * @brief Print usage examples
 */
void print_examples() {
    std::cout << "Examples:\n";
    std::cout << "  file-processor document.txt                    # Basic file analysis\n";
    std::cout << "  file-processor --count *.txt                  # Count statistics\n";
    std::cout << "  file-processor --search \"TODO\" src/*.cpp      # Search patterns\n";
    std::cout << "  file-processor --format json --output out.json *.md  # JSON output\n";
    std::cout << "  file-processor --recursive --include \"*.cpp,*.h\" src/  # Recursive\n\n";
}

/**
 * @brief Validate input files and directories
 * @param files List of file paths to validate
 * @param recursive Whether to process directories recursively
 * @return Vector of valid file paths
 */
std::vector<std::string> validate_files(const std::vector<std::string>& files, bool recursive) {
    std::vector<std::string> valid_files;
    
    for (const auto& file_path : files) {
        fs::path path(file_path);
        
        if (!fs::exists(path)) {
            utils::Logger::warn("File does not exist: {}", file_path);
            continue;
        }
        
        if (fs::is_regular_file(path)) {
            valid_files.push_back(file_path);
        } else if (fs::is_directory(path) && recursive) {
            // Add directory for recursive processing
            valid_files.push_back(file_path);
        } else if (fs::is_directory(path)) {
            utils::Logger::warn("Skipping directory (use --recursive): {}", file_path);
        } else {
            utils::Logger::warn("Skipping non-regular file: {}", file_path);
        }
    }
    
    return valid_files;
}

/**
 * @brief Main application entry point
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit code (0 for success, non-zero for error)
 */
int main(int argc, char* argv[]) {
    try {
        // Initialize logger
        utils::Logger::init();
        
        // Parse command-line arguments
        cli::ArgumentParser parser("file-processor", "A powerful file processing utility");
        
        // Simple argument parsing for demonstration
        bool show_help = false;
        bool show_version = false;
        bool show_examples = false;
        bool count_mode = false;
        bool verbose = false;
        std::string search_pattern;
        std::vector<std::string> files;

        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                show_help = true;
            } else if (arg == "--version") {
                show_version = true;
            } else if (arg == "--examples") {
                show_examples = true;
            } else if (arg == "--count" || arg == "-c") {
                count_mode = true;
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--search" || arg == "-s") {
                if (i + 1 < argc) {
                    search_pattern = argv[++i];
                }
            } else if (arg.empty() || arg[0] != '-') {
                files.push_back(arg);
            }
        }
        
        // Handle special options
        if (show_help) {
            std::cout << "Usage: " << argv[0] << " [OPTIONS] FILES...\n\n";
            std::cout << "Options:\n";
            std::cout << "  -h, --help      Show this help message\n";
            std::cout << "  --version       Show version information\n";
            std::cout << "  --examples      Show usage examples\n";
            std::cout << "  -c, --count     Count lines, words, characters\n";
            std::cout << "  -s, --search    Search for pattern in files\n";
            std::cout << "  -v, --verbose   Enable verbose output\n";
            return 0;
        }

        if (show_version) {
            print_banner();
            return 0;
        }

        if (show_examples) {
            print_banner();
            print_examples();
            return 0;
        }

        // Configure logging
        utils::Logger::set_level(verbose ? "debug" : "info");
        
        // Check if files were provided
        if (files.empty()) {
            std::cerr << "Error: No files specified. Use --help for usage information.\n";
            return 1;
        }

        // Validate input files
        auto valid_files = validate_files(files, false); // recursive = false for simplicity
        if (valid_files.empty()) {
            std::cerr << "Error: No valid files to process\n";
            return 1;
        }

        utils::Logger::info("Processing {} files", valid_files.size());

        // Create file processor with basic configuration
        core::FileProcessor::Config config;
        config.format = "text";
        config.recursive = false;

        core::FileProcessor processor(config);

        // Process files based on operation mode
        bool success = false;

        if (count_mode) {
            success = processor.count_files(valid_files);
        } else if (!search_pattern.empty()) {
            success = processor.search_files(valid_files, search_pattern);
        } else {
            success = processor.process_files(valid_files);
        }

        if (!success) {
            std::cerr << "Error: File processing failed\n";
            return 1;
        }

        utils::Logger::info("File processing completed successfully");
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        utils::Logger::error("Unhandled exception: {}", e.what());
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        utils::Logger::error("Unknown exception caught");
        return 1;
    }
}
