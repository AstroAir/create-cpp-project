# Console Application Tutorial

In this tutorial, you'll create a complete command-line file processing utility that demonstrates best practices for console applications.

## What You'll Build

A file processing utility called "FileProcessor" that can:

- Count lines, words, and characters in text files
- Search for patterns in files
- Convert between different text encodings
- Process multiple files with batch operations
- Provide detailed logging and error reporting

## Prerequisites

- CPP-Scaffold installed
- C++17 compatible compiler
- Basic command-line knowledge
- Text editor or IDE

## Step 1: Create the Project

Create a new console application project:

```bash
cpp-scaffold FileProcessor \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode
```

This creates a console application with:

- CMake build system
- vcpkg package management
- Google Test for unit testing
- GitHub Actions CI/CD
- VSCode configuration

## Step 2: Explore the Generated Structure

Navigate to the project and examine the structure:

```bash
cd FileProcessor
tree
```

You'll see:

```
FileProcessor/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── .clang-format
├── vcpkg.json
├── .github/
│   └── workflows/
│       └── ci.yml
├── .vscode/
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
├── src/
│   ├── main.cpp
│   ├── cli/
│   │   ├── argument_parser.h
│   │   └── argument_parser.cpp
│   └── core/
│       ├── file_processor.h
│       └── file_processor.cpp
├── include/
│   └── FileProcessor/
│       └── version.h
└── tests/
    ├── CMakeLists.txt
    └── test_file_processor.cpp
```

## Step 3: Examine the Generated Code

Look at the main application entry point:

```cpp title="src/main.cpp"
#include <iostream>
#include <string>
#include <vector>
#include "cli/argument_parser.h"
#include "core/file_processor.h"

int main(int argc, char* argv[]) {
    try {
        cli::ArgumentParser parser("FileProcessor", "A powerful file processing utility");

        // Add command-line options
        parser.add_argument("files")
            .help("Input files to process")
            .nargs(argparse::nargs_pattern::at_least_one);

        parser.add_argument("-c", "--count")
            .help("Count lines, words, and characters")
            .default_value(false)
            .implicit_value(true);

        parser.add_argument("-s", "--search")
            .help("Search for pattern in files")
            .default_value(std::string{});

        parser.add_argument("-v", "--verbose")
            .help("Enable verbose output")
            .default_value(false)
            .implicit_value(true);

        // Parse arguments
        parser.parse_args(argc, argv);

        // Get parsed values
        auto files = parser.get<std::vector<std::string>>("files");
        auto count_mode = parser.get<bool>("--count");
        auto search_pattern = parser.get<std::string>("--search");
        auto verbose = parser.get<bool>("--verbose");

        // Create file processor
        core::FileProcessor processor(verbose);

        // Process files based on options
        if (count_mode) {
            processor.count_files(files);
        } else if (!search_pattern.empty()) {
            processor.search_files(files, search_pattern);
        } else {
            processor.process_files(files);
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Step 4: Implement the Core Functionality

Update the file processor header:

```cpp title="src/core/file_processor.h"
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace core {
    struct FileStats {
        size_t lines = 0;
        size_t words = 0;
        size_t characters = 0;
    };

    class FileProcessor {
    public:
        explicit FileProcessor(bool verbose = false);

        void process_files(const std::vector<std::string>& files);
        void count_files(const std::vector<std::string>& files);
        void search_files(const std::vector<std::string>& files, const std::string& pattern);

    private:
        bool verbose_;

        FileStats count_file(const std::string& filename);
        std::vector<std::string> search_file(const std::string& filename, const std::string& pattern);
        void log(const std::string& message);
    };
}
```

And the implementation:

```cpp title="src/core/file_processor.cpp"
#include "core/file_processor.h"
#include <sstream>
#include <regex>
#include <iomanip>

namespace core {
    FileProcessor::FileProcessor(bool verbose) : verbose_(verbose) {}

    void FileProcessor::process_files(const std::vector<std::string>& files) {
        log("Processing " + std::to_string(files.size()) + " files...");

        for (const auto& file : files) {
            log("Processing: " + file);
            auto stats = count_file(file);

            std::cout << file << ":\n";
            std::cout << "  Lines: " << stats.lines << "\n";
            std::cout << "  Words: " << stats.words << "\n";
            std::cout << "  Characters: " << stats.characters << "\n\n";
        }
    }

    void FileProcessor::count_files(const std::vector<std::string>& files) {
        log("Counting statistics for " + std::to_string(files.size()) + " files...");

        FileStats total{};

        std::cout << std::left << std::setw(20) << "File"
                  << std::setw(10) << "Lines"
                  << std::setw(10) << "Words"
                  << std::setw(12) << "Characters" << "\n";
        std::cout << std::string(52, '-') << "\n";

        for (const auto& file : files) {
            auto stats = count_file(file);
            total.lines += stats.lines;
            total.words += stats.words;
            total.characters += stats.characters;

            std::cout << std::left << std::setw(20) << file
                      << std::setw(10) << stats.lines
                      << std::setw(10) << stats.words
                      << std::setw(12) << stats.characters << "\n";
        }

        std::cout << std::string(52, '-') << "\n";
        std::cout << std::left << std::setw(20) << "TOTAL"
                  << std::setw(10) << total.lines
                  << std::setw(10) << total.words
                  << std::setw(12) << total.characters << "\n";
    }

    void FileProcessor::search_files(const std::vector<std::string>& files, const std::string& pattern) {
        log("Searching for pattern '" + pattern + "' in " + std::to_string(files.size()) + " files...");

        std::regex search_regex(pattern);

        for (const auto& file : files) {
            auto matches = search_file(file, pattern);

            if (!matches.empty()) {
                std::cout << file << " (" << matches.size() << " matches):\n";
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << "  " << (i + 1) << ": " << matches[i] << "\n";
                }
                std::cout << "\n";
            }
        }
    }

    FileStats FileProcessor::count_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        FileStats stats{};
        std::string line;

        while (std::getline(file, line)) {
            stats.lines++;
            stats.characters += line.length() + 1; // +1 for newline

            std::istringstream iss(line);
            std::string word;
            while (iss >> word) {
                stats.words++;
            }
        }

        return stats;
    }

    std::vector<std::string> FileProcessor::search_file(const std::string& filename, const std::string& pattern) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::vector<std::string> matches;
        std::string line;
        std::regex search_regex(pattern);

        while (std::getline(file, line)) {
            if (std::regex_search(line, search_regex)) {
                matches.push_back(line);
            }
        }

        return matches;
    }

    void FileProcessor::log(const std::string& message) {
        if (verbose_) {
            std::cout << "[LOG] " << message << std::endl;
        }
    }
}
```

## Step 5: Build and Test

Build the project:

```bash
mkdir build && cd build
cmake .. -G Ninja
ninja
```

Test the application:

```bash
# Create test files
echo -e "Hello world\nThis is a test\nAnother line" > test1.txt
echo -e "More content\nTesting 123\nFinal line" > test2.txt

# Count statistics
./FileProcessor --count test1.txt test2.txt

# Search for patterns
./FileProcessor --search "test" test1.txt test2.txt

# Verbose mode
./FileProcessor --verbose --count test1.txt
```

## Step 6: Add Unit Tests

Update the test file:

```cpp title="tests/test_file_processor.cpp"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "core/file_processor.h"

class FileProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test file
        std::ofstream file("test_file.txt");
        file << "Line 1\n";
        file << "Line 2 with more words\n";
        file << "Final line\n";
        file.close();
    }

    void TearDown() override {
        std::filesystem::remove("test_file.txt");
    }
};

TEST_F(FileProcessorTest, CountFile) {
    core::FileProcessor processor;

    // This would require making count_file public or adding a friend class
    // For now, test through the public interface
    std::vector<std::string> files = {"test_file.txt"};

    // Test that it doesn't throw
    EXPECT_NO_THROW(processor.count_files(files));
}

TEST_F(FileProcessorTest, SearchFile) {
    core::FileProcessor processor;
    std::vector<std::string> files = {"test_file.txt"};

    // Test that search doesn't throw
    EXPECT_NO_THROW(processor.search_files(files, "Line"));
}

TEST_F(FileProcessorTest, ProcessFiles) {
    core::FileProcessor processor;
    std::vector<std::string> files = {"test_file.txt"};

    // Test that process doesn't throw
    EXPECT_NO_THROW(processor.process_files(files));
}
```

Run the tests:

```bash
ctest --verbose
```

## Step 7: Usage Examples

Your file processor is now ready! Here are some usage examples:

```bash
# Basic file processing
./FileProcessor document.txt

# Count statistics for multiple files
./FileProcessor --count *.txt

# Search for patterns
./FileProcessor --search "TODO" src/*.cpp

# Verbose output
./FileProcessor --verbose --count README.md
```

## What You've Learned

✅ Created a complete console application with CPP-Scaffold  
✅ Implemented command-line argument parsing  
✅ Built file processing functionality  
✅ Added comprehensive error handling  
✅ Created unit tests  
✅ Used modern C++ features and best practices

## Next Steps

- **Add more features**: File encoding conversion, output formatting
- **Improve error handling**: Better error messages and recovery
- **Add configuration files**: Support for config files and profiles
- **Performance optimization**: Handle large files efficiently
- **Cross-platform testing**: Test on different operating systems

## Related Tutorials

- [Library Project Tutorial](library-project.md) - Create reusable components
- [Advanced Features Tutorial](advanced-features.md) - Custom templates and CI/CD
