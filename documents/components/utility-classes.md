# Utility Classes Component

The Utility Classes component provides cross-platform utilities and helper functions that support the core functionality of CPP-Scaffold.

## 📋 Overview

The Utility Classes component includes:
- Cross-platform file system operations
- String manipulation and text processing utilities
- Terminal styling and interactive user interfaces
- Git repository management and version control
- Framework integration and third-party library setup
- Progress tracking and user feedback mechanisms

## 🏗️ Architecture

### Component Structure

```
src/utils/
├── file_utils.h/.cpp              # File system operations
├── string_utils.h/.cpp            # String manipulation
├── terminal_utils.h/.cpp          # Terminal styling and interaction
├── git_utils.h/.cpp               # Git operations
├── framework_integration.h/.cpp   # Third-party framework setup
├── progress_indicator.h/.cpp      # Progress visualization
├── interactive_menu.h/.cpp        # Interactive user interfaces
├── dependency_manager.h/.cpp      # Dependency management
└── post_creation_actions.h/.cpp   # Post-project-creation tasks
```

### Utility Categories

```cpp
namespace utils {
    // File system utilities
    class FileUtils;
    
    // String processing utilities
    class StringUtils;
    
    // Terminal interaction utilities
    class TerminalUtils;
    
    // Git integration utilities
    class GitUtils;
    
    // Framework integration utilities
    class FrameworkIntegration;
    
    // Progress tracking utilities
    class ProgressIndicator;
    
    // Interactive menu utilities
    class InteractiveMenu;
    
    // Dependency management utilities
    class DependencyManager;
    
    // Post-creation action utilities
    class PostCreationActions;
}
```

## 🔧 Core Utility Classes

### FileUtils

Cross-platform file system operations with error handling and logging.

```cpp
namespace utils {
class FileUtils {
public:
    // Directory operations
    static bool createDirectory(const std::filesystem::path& path);
    static bool createDirectoryStructure(const std::vector<std::filesystem::path>& paths,
                                       const std::filesystem::path& basePath);
    static bool directoryExists(const std::filesystem::path& path);
    static bool isDirectoryEmpty(const std::filesystem::path& path);
    static std::vector<std::filesystem::path> listFiles(const std::filesystem::path& directory,
                                                       bool recursive = false);
    static std::vector<std::filesystem::path> listDirectories(const std::filesystem::path& directory);
    static bool removeDirectory(const std::filesystem::path& path, bool recursive = false);
    
    // File operations
    static bool writeToFile(const std::filesystem::path& filePath, const std::string& content);
    static std::string readFromFile(const std::filesystem::path& filePath);
    static bool fileExists(const std::filesystem::path& path);
    static bool copyFile(const std::filesystem::path& source, const std::filesystem::path& destination);
    static bool copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination);
    static bool moveFile(const std::filesystem::path& source, const std::filesystem::path& destination);
    static bool removeFile(const std::filesystem::path& path);
    static std::uintmax_t getFileSize(const std::filesystem::path& path);
    static std::filesystem::file_time_type getLastWriteTime(const std::filesystem::path& path);
    
    // Path utilities
    static std::string getFileName(const std::filesystem::path& path);
    static std::string getFileNameWithoutExtension(const std::filesystem::path& path);
    static std::string getFileExtension(const std::filesystem::path& path);
    static std::filesystem::path getParentPath(const std::filesystem::path& path);
    static std::filesystem::path getAbsolutePath(const std::filesystem::path& path);
    static std::filesystem::path getRelativePath(const std::filesystem::path& from, 
                                                const std::filesystem::path& to);
    static std::filesystem::path combinePath(const std::filesystem::path& path1, 
                                            const std::filesystem::path& path2);
    
    // Permissions and attributes
    static bool setExecutable(const std::filesystem::path& filePath);
    static bool isExecutable(const std::filesystem::path& filePath);
    static bool isReadable(const std::filesystem::path& filePath);
    static bool isWritable(const std::filesystem::path& filePath);
    static bool setPermissions(const std::filesystem::path& filePath, 
                              std::filesystem::perms permissions);
    
    // System utilities
    static std::filesystem::path getTempDirectory();
    static std::filesystem::path getCurrentDirectory();
    static bool setCurrentDirectory(const std::filesystem::path& path);
    static std::filesystem::path getHomeDirectory();
    static std::filesystem::path getExecutablePath();
    
    // File content utilities
    static bool appendToFile(const std::filesystem::path& filePath, const std::string& content);
    static std::vector<std::string> readLines(const std::filesystem::path& filePath);
    static bool writeLines(const std::filesystem::path& filePath, 
                          const std::vector<std::string>& lines);
    static bool replaceInFile(const std::filesystem::path& filePath, 
                             const std::string& search, 
                             const std::string& replace);
    
    // Validation
    static bool isValidPath(const std::string& path);
    static bool isValidFileName(const std::string& fileName);
    static bool hasWritePermission(const std::filesystem::path& path);
};
}
```

### StringUtils

String manipulation and text processing utilities.

```cpp
namespace utils {
class StringUtils {
public:
    // Case conversion
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string toTitleCase(const std::string& str);
    static std::string toCamelCase(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toKebabCase(const std::string& str);
    static std::string toPascalCase(const std::string& str);
    
    // String manipulation
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string reverse(const std::string& str);
    
    // Whitespace handling
    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string removeWhitespace(const std::string& str);
    static std::string normalizeWhitespace(const std::string& str);
    
    // String queries
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool contains(const std::string& str, const std::string& substring);
    static bool isEmpty(const std::string& str);
    static bool isWhitespace(const std::string& str);
    static bool isNumeric(const std::string& str);
    static bool isAlphabetic(const std::string& str);
    static bool isAlphanumeric(const std::string& str);
    
    // Validation
    static bool isValidIdentifier(const std::string& str);
    static bool isValidProjectName(const std::string& str);
    static bool isValidEmail(const std::string& str);
    static bool isValidUrl(const std::string& str);
    static bool isValidVersion(const std::string& str);
    
    // Formatting
    static std::string format(const std::string& format, const std::vector<std::string>& args);
    static std::string padLeft(const std::string& str, size_t width, char fillChar = ' ');
    static std::string padRight(const std::string& str, size_t width, char fillChar = ' ');
    static std::string center(const std::string& str, size_t width, char fillChar = ' ');
    static std::string repeat(const std::string& str, size_t count);
    
    // Encoding and escaping
    static std::string escapeJson(const std::string& str);
    static std::string unescapeJson(const std::string& str);
    static std::string escapeXml(const std::string& str);
    static std::string unescapeXml(const std::string& str);
    static std::string escapeShell(const std::string& str);
    
    // Comparison
    static bool equalsIgnoreCase(const std::string& str1, const std::string& str2);
    static int compareIgnoreCase(const std::string& str1, const std::string& str2);
    static bool isSubsequence(const std::string& str, const std::string& subsequence);
    
    // Utility functions
    static std::string generateRandomString(size_t length, const std::string& charset = "");
    static std::string generateUuid();
    static std::string getCurrentTimestamp(const std::string& format = "%Y-%m-%d %H:%M:%S");
};
}
```

### TerminalUtils

Terminal styling and interactive user interface utilities.

```cpp
namespace utils {
class TerminalUtils {
public:
    // Color codes
    enum class Color {
        Black, Red, Green, Yellow, Blue, Magenta, Cyan, White,
        BrightBlack, BrightRed, BrightGreen, BrightYellow,
        BrightBlue, BrightMagenta, BrightCyan, BrightWhite,
        Default
    };
    
    enum class Style {
        Reset, Bold, Dim, Italic, Underline, Blink, Reverse, Strikethrough
    };
    
    // Terminal capabilities
    static bool supportsColor();
    static bool supportsUnicode();
    static bool isInteractive();
    static std::pair<int, int> getTerminalSize();
    
    // Text styling
    static std::string colorize(const std::string& text, Color foreground, Color background = Color::Default);
    static std::string style(const std::string& text, Style style);
    static std::string bold(const std::string& text);
    static std::string italic(const std::string& text);
    static std::string underline(const std::string& text);
    static std::string reset(const std::string& text);
    
    // Predefined styles
    static std::string success(const std::string& text);
    static std::string error(const std::string& text);
    static std::string warning(const std::string& text);
    static std::string info(const std::string& text);
    static std::string highlight(const std::string& text);
    static std::string muted(const std::string& text);
    
    // Output utilities
    static void clearScreen();
    static void clearLine();
    static void moveCursor(int row, int col);
    static void hideCursor();
    static void showCursor();
    static void saveCursor();
    static void restoreCursor();
    
    // Input utilities
    static std::string readLine(const std::string& prompt = "");
    static char readChar();
    static bool readYesNo(const std::string& prompt, bool defaultValue = false);
    static std::string readPassword(const std::string& prompt = "Password: ");
    
    // Progress and status
    static void showSpinner(const std::string& message = "");
    static void hideSpinner();
    static void updateSpinner(const std::string& message);
    
    // Box drawing and formatting
    static std::string drawBox(const std::string& content, int width = 0);
    static std::string drawLine(int width, char character = '-');
    static std::string drawTable(const std::vector<std::vector<std::string>>& data,
                                const std::vector<std::string>& headers = {});
    
    // Logging and output
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printDebug(const std::string& message);
    
private:
    static std::string getColorCode(Color color, bool background = false);
    static std::string getStyleCode(Style style);
    static bool colorEnabled_;
    static bool spinnerActive_;
};
}
```

## 💡 Usage Examples

### File Operations

```cpp
#include "utils/file_utils.h"

void createProjectStructure() {
    using namespace utils;
    
    // Create directory structure
    std::vector<std::filesystem::path> directories = {
        "src", "tests", "docs", "cmake", "scripts"
    };
    
    if (FileUtils::createDirectoryStructure(directories, "MyProject")) {
        std::cout << "Project structure created successfully" << std::endl;
    }
    
    // Create files
    std::string mainContent = R"(#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
)";
    
    FileUtils::writeToFile("MyProject/src/main.cpp", mainContent);
    
    // Set executable permissions
    FileUtils::setExecutable("MyProject/build.sh");
}
```

### String Processing

```cpp
#include "utils/string_utils.h"

void processProjectName() {
    using namespace utils;
    
    std::string userInput = "  My-Cool_Project  ";
    
    // Clean and validate
    std::string cleaned = StringUtils::trim(userInput);
    
    if (StringUtils::isValidProjectName(cleaned)) {
        // Convert to different cases
        std::string camelCase = StringUtils::toCamelCase(cleaned);
        std::string snakeCase = StringUtils::toSnakeCase(cleaned);
        std::string kebabCase = StringUtils::toKebabCase(cleaned);
        
        std::cout << "CamelCase: " << camelCase << std::endl;
        std::cout << "snake_case: " << snakeCase << std::endl;
        std::cout << "kebab-case: " << kebabCase << std::endl;
    }
}
```

### Terminal Styling

```cpp
#include "utils/terminal_utils.h"

void showStyledOutput() {
    using namespace utils;
    
    // Check terminal capabilities
    if (TerminalUtils::supportsColor()) {
        // Styled output
        TerminalUtils::printSuccess("Project created successfully!");
        TerminalUtils::printWarning("Some dependencies are missing");
        TerminalUtils::printError("Build failed");
        
        // Custom styling
        std::string title = TerminalUtils::bold(
            TerminalUtils::colorize("CPP-Scaffold", TerminalUtils::Color::Blue)
        );
        std::cout << title << std::endl;
        
        // Draw a box
        std::string boxContent = "Welcome to CPP-Scaffold!\nYour project is ready.";
        std::cout << TerminalUtils::drawBox(boxContent, 40) << std::endl;
    }
}
```

## 🧪 Testing

### Utility Testing Strategy

```cpp
// Test file operations
TEST(FileUtilsTest, CreateAndReadFile) {
    const std::string testContent = "Hello, World!";
    const std::filesystem::path testFile = "test_file.txt";
    
    EXPECT_TRUE(utils::FileUtils::writeToFile(testFile, testContent));
    EXPECT_TRUE(utils::FileUtils::fileExists(testFile));
    
    std::string readContent = utils::FileUtils::readFromFile(testFile);
    EXPECT_EQ(readContent, testContent);
    
    EXPECT_TRUE(utils::FileUtils::removeFile(testFile));
}

// Test string utilities
TEST(StringUtilsTest, CaseConversion) {
    EXPECT_EQ(utils::StringUtils::toCamelCase("hello_world"), "helloWorld");
    EXPECT_EQ(utils::StringUtils::toSnakeCase("HelloWorld"), "hello_world");
    EXPECT_EQ(utils::StringUtils::toKebabCase("HelloWorld"), "hello-world");
}

// Test validation
TEST(StringUtilsTest, ProjectNameValidation) {
    EXPECT_TRUE(utils::StringUtils::isValidProjectName("MyProject"));
    EXPECT_TRUE(utils::StringUtils::isValidProjectName("my_project"));
    EXPECT_FALSE(utils::StringUtils::isValidProjectName("123project"));
    EXPECT_FALSE(utils::StringUtils::isValidProjectName(""));
}
```

## 🔗 Dependencies

### External Dependencies
- **std::filesystem**: File system operations (C++17)
- **spdlog**: Logging and error reporting
- **Standard Library**: String processing, I/O operations

### Internal Dependencies
- **Configuration System**: For utility preferences and settings
- **CLI Parser**: For command-line utility integration

## 📊 Performance Considerations

- **File operations**: Optimized for batch operations
- **String processing**: Efficient algorithms with minimal allocations
- **Terminal output**: Buffered output for better performance
- **Memory usage**: RAII and smart pointer usage throughout

## 🔧 Extension Points

### Adding New Utilities

1. **Create utility class** in appropriate namespace
2. **Follow naming conventions** and coding standards
3. **Add comprehensive tests** for all functionality
4. **Update documentation** with usage examples
5. **Consider cross-platform compatibility**

### Custom Utility Integration

```cpp
// Register custom utility
namespace utils {
    class MyCustomUtils {
    public:
        static bool myCustomFunction(const std::string& input);
    };
}

// Use in templates or other components
if (utils::MyCustomUtils::myCustomFunction(userInput)) {
    // Handle success
}
```

---

For more information, see the [API Reference](../api-reference/API.md) and [Architecture Documentation](../developer-guide/ARCHITECTURE.md).
