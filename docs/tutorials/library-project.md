# Library Project Tutorial

Learn to create a reusable C++ library with proper API design, comprehensive testing, and packaging support.

## What You'll Build

A string manipulation library called "StringUtils" that provides:

- String trimming and padding functions
- Case conversion utilities
- String splitting and joining
- Pattern matching and replacement
- Comprehensive unit tests
- API documentation
- Package configuration for easy integration

## Prerequisites

- CPP-Scaffold installed
- C++17 compatible compiler
- Basic understanding of library design
- Familiarity with CMake

## Step 1: Create the Library Project

```bash
cpp-scaffold StringUtils \
  --type lib \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode
```

## Step 2: Design the Public API

Update the main header file:

```cpp title="include/StringUtils/StringUtils.h"
#pragma once

#include <string>
#include <vector>
#include <regex>

namespace StringUtils {
    // Version information
    constexpr int VERSION_MAJOR = 1;
    constexpr int VERSION_MINOR = 0;
    constexpr int VERSION_PATCH = 0;
    
    // Trimming functions
    std::string trim_left(const std::string& str);
    std::string trim_right(const std::string& str);
    std::string trim(const std::string& str);
    
    // Padding functions
    std::string pad_left(const std::string& str, size_t width, char fill = ' ');
    std::string pad_right(const std::string& str, size_t width, char fill = ' ');
    std::string pad_center(const std::string& str, size_t width, char fill = ' ');
    
    // Case conversion
    std::string to_upper(const std::string& str);
    std::string to_lower(const std::string& str);
    std::string to_title_case(const std::string& str);
    
    // String splitting and joining
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    std::string join(const std::vector<std::string>& strings, const std::string& separator);
    
    // Pattern operations
    bool contains(const std::string& str, const std::string& pattern);
    std::string replace_all(const std::string& str, const std::string& from, const std::string& to);
    std::vector<std::string> find_matches(const std::string& str, const std::regex& pattern);
    
    // Utility functions
    bool starts_with(const std::string& str, const std::string& prefix);
    bool ends_with(const std::string& str, const std::string& suffix);
    size_t count_occurrences(const std::string& str, const std::string& substring);
}
```

## Step 3: Implement the Library

```cpp title="src/StringUtils.cpp"
#include "StringUtils/StringUtils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace StringUtils {
    std::string trim_left(const std::string& str) {
        auto start = str.find_first_not_of(" \t\n\r\f\v");
        return (start == std::string::npos) ? "" : str.substr(start);
    }
    
    std::string trim_right(const std::string& str) {
        auto end = str.find_last_not_of(" \t\n\r\f\v");
        return (end == std::string::npos) ? "" : str.substr(0, end + 1);
    }
    
    std::string trim(const std::string& str) {
        return trim_left(trim_right(str));
    }
    
    std::string pad_left(const std::string& str, size_t width, char fill) {
        if (str.length() >= width) return str;
        return std::string(width - str.length(), fill) + str;
    }
    
    std::string pad_right(const std::string& str, size_t width, char fill) {
        if (str.length() >= width) return str;
        return str + std::string(width - str.length(), fill);
    }
    
    std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = str.find(delimiter);
        
        while (end != std::string::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }
        
        tokens.push_back(str.substr(start));
        return tokens;
    }
    
    std::string join(const std::vector<std::string>& strings, const std::string& separator) {
        if (strings.empty()) return "";
        
        std::ostringstream result;
        result << strings[0];
        
        for (size_t i = 1; i < strings.size(); ++i) {
            result << separator << strings[i];
        }
        
        return result.str();
    }
    
    bool contains(const std::string& str, const std::string& pattern) {
        return str.find(pattern) != std::string::npos;
    }
    
    std::string replace_all(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        
        return result;
    }
    
    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.length() >= prefix.length() && 
               str.substr(0, prefix.length()) == prefix;
    }
    
    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.length() >= suffix.length() && 
               str.substr(str.length() - suffix.length()) == suffix;
    }
}
```

## Step 4: Create Comprehensive Tests

```cpp title="tests/test_string_utils.cpp"
#include <gtest/gtest.h>
#include "StringUtils/StringUtils.h"

class StringUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(StringUtilsTest, TrimFunctions) {
    EXPECT_EQ(StringUtils::trim("  hello  "), "hello");
    EXPECT_EQ(StringUtils::trim_left("  hello  "), "hello  ");
    EXPECT_EQ(StringUtils::trim_right("  hello  "), "  hello");
    EXPECT_EQ(StringUtils::trim(""), "");
    EXPECT_EQ(StringUtils::trim("   "), "");
}

TEST_F(StringUtilsTest, PaddingFunctions) {
    EXPECT_EQ(StringUtils::pad_left("hello", 10), "     hello");
    EXPECT_EQ(StringUtils::pad_right("hello", 10), "hello     ");
    EXPECT_EQ(StringUtils::pad_left("hello", 10, '*'), "*****hello");
    EXPECT_EQ(StringUtils::pad_left("hello", 3), "hello"); // No padding needed
}

TEST_F(StringUtilsTest, CaseConversion) {
    EXPECT_EQ(StringUtils::to_upper("hello"), "HELLO");
    EXPECT_EQ(StringUtils::to_lower("HELLO"), "hello");
    EXPECT_EQ(StringUtils::to_upper("Hello World"), "HELLO WORLD");
}

TEST_F(StringUtilsTest, SplitAndJoin) {
    auto parts = StringUtils::split("a,b,c", ",");
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
    
    EXPECT_EQ(StringUtils::join({"a", "b", "c"}, ","), "a,b,c");
    EXPECT_EQ(StringUtils::join({}, ","), "");
    EXPECT_EQ(StringUtils::join({"single"}, ","), "single");
}

TEST_F(StringUtilsTest, PatternOperations) {
    EXPECT_TRUE(StringUtils::contains("hello world", "world"));
    EXPECT_FALSE(StringUtils::contains("hello world", "xyz"));
    
    EXPECT_EQ(StringUtils::replace_all("hello hello", "hello", "hi"), "hi hi");
    EXPECT_EQ(StringUtils::replace_all("test", "xyz", "abc"), "test");
}

TEST_F(StringUtilsTest, UtilityFunctions) {
    EXPECT_TRUE(StringUtils::starts_with("hello world", "hello"));
    EXPECT_FALSE(StringUtils::starts_with("hello world", "world"));
    
    EXPECT_TRUE(StringUtils::ends_with("hello world", "world"));
    EXPECT_FALSE(StringUtils::ends_with("hello world", "hello"));
}
```

## Step 5: Build and Test

```bash
mkdir build && cd build
cmake .. -G Ninja
ninja

# Run tests
ctest --verbose

# Run specific tests
./tests/test_string_utils
```

## Step 6: Create Usage Example

```cpp title="examples/example_usage.cpp"
#include <iostream>
#include "StringUtils/StringUtils.h"

int main() {
    std::cout << "StringUtils Library Demo\n";
    std::cout << "========================\n\n";
    
    // Trimming
    std::string text = "  Hello, World!  ";
    std::cout << "Original: '" << text << "'\n";
    std::cout << "Trimmed: '" << StringUtils::trim(text) << "'\n\n";
    
    // Case conversion
    std::cout << "Uppercase: " << StringUtils::to_upper("hello world") << "\n";
    std::cout << "Lowercase: " << StringUtils::to_lower("HELLO WORLD") << "\n\n";
    
    // Splitting and joining
    auto parts = StringUtils::split("apple,banana,cherry", ",");
    std::cout << "Split result:\n";
    for (const auto& part : parts) {
        std::cout << "  - " << part << "\n";
    }
    
    std::cout << "Joined: " << StringUtils::join(parts, " | ") << "\n\n";
    
    // Pattern operations
    std::string sentence = "The quick brown fox jumps over the lazy dog";
    std::cout << "Contains 'fox': " << StringUtils::contains(sentence, "fox") << "\n";
    std::cout << "Replaced: " << StringUtils::replace_all(sentence, "fox", "cat") << "\n";
    
    return 0;
}
```

## Step 7: Package Configuration

The generated CMakeLists.txt already includes proper export configuration. You can install and use the library:

```bash
# Install the library
ninja install

# Use in another project
find_package(StringUtils REQUIRED)
target_link_libraries(my_app StringUtils::StringUtils)
```

## What You've Learned

✅ Designed a clean, reusable API  
✅ Implemented comprehensive functionality  
✅ Created thorough unit tests  
✅ Built proper library packaging  
✅ Generated usage examples  
✅ Followed modern C++ best practices  

## Next Steps

- **Add more features**: Regular expression support, Unicode handling
- **Performance optimization**: Benchmark and optimize critical functions
- **Documentation**: Generate API docs with Doxygen
- **Packaging**: Create packages for vcpkg/Conan
- **Versioning**: Implement semantic versioning

## Related Tutorials

- [Console Application Tutorial](console-app.md) - Build command-line tools
- [Advanced Features Tutorial](advanced-features.md) - Custom templates and CI/CD
