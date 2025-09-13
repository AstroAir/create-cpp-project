# Contributing to CPP-Scaffold

Thank you for your interest in contributing to CPP-Scaffold! This document provides guidelines and information for contributors.

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Contributing Guidelines](#contributing-guidelines)
- [Code Style](#code-style)
- [Testing](#testing)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)
- [Issue Reporting](#issue-reporting)

## 🤝 Code of Conduct

### Our Pledge

We are committed to making participation in our project a harassment-free experience for everyone, regardless of age, body size, disability, ethnicity, gender identity and expression, level of experience, nationality, personal appearance, race, religion, or sexual identity and orientation.

### Our Standards

**Positive behavior includes:**

- Using welcoming and inclusive language
- Being respectful of differing viewpoints and experiences
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

**Unacceptable behavior includes:**

- The use of sexualized language or imagery
- Trolling, insulting/derogatory comments, and personal or political attacks
- Public or private harassment
- Publishing others' private information without explicit permission
- Other conduct which could reasonably be considered inappropriate in a professional setting

## 🚀 Getting Started

### Prerequisites

Before contributing, ensure you have:

- **C++17 compatible compiler** (GCC 7+, Clang 6+, MSVC 2017+)
- **CMake 3.14+**
- **Git**
- **Basic understanding of C++ and CMake**
- **Familiarity with the project architecture** (see [Architecture Guide](architecture.md))

### Areas for Contribution

We welcome contributions in the following areas:

1. **New Project Templates**

   - Additional project types (game engines, mobile apps, etc.)
   - Framework-specific templates
   - Domain-specific templates

2. **Build System Support**

   - Additional build systems (Bazel, XMake, etc.)
   - Build system optimizations
   - Cross-compilation support

3. **Package Manager Integration**

   - New package managers
   - Improved dependency resolution
   - Package manager optimizations

4. **IDE Integration**

   - Additional IDE support
   - Enhanced project configurations
   - Plugin development

5. **Documentation**

   - User guides and tutorials
   - API documentation
   - Example projects

6. **Testing and Quality Assurance**
   - Unit tests
   - Integration tests
   - Performance tests
   - Bug fixes

## 🛠️ Development Setup

### 1. Fork and Clone

```bash
# Fork the repository on GitHub
# Then clone your fork
git clone https://github.com/YOUR_USERNAME/cpp-scaffold.git
cd cpp-scaffold

# Add upstream remote
git remote add upstream https://github.com/cpp-scaffold/cpp-scaffold.git
```

### 2. Build Development Environment

```bash
# Create development build
mkdir build-dev && cd build-dev
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
ninja

# Run tests to ensure everything works
ctest
```

### 3. Development Tools Setup

```bash
# Install development tools (optional but recommended)
# clang-format for code formatting
sudo apt install clang-format  # Ubuntu
brew install clang-format      # macOS

# clang-tidy for static analysis
sudo apt install clang-tidy    # Ubuntu
brew install llvm               # macOS (includes clang-tidy)
```

### 4. IDE Configuration

#### Visual Studio Code

```bash
# Install recommended extensions
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension xaver.clang-format
```

#### CLion

- Open the project directory
- CLion will automatically detect CMake configuration
- Enable clang-format integration in settings

## 📝 Contributing Guidelines

### 1. Branch Strategy

```bash
# Create feature branch from main
git checkout main
git pull upstream main
git checkout -b feature/your-feature-name

# Or for bug fixes
git checkout -b fix/issue-description
```

### 2. Commit Guidelines

#### Commit Message Format

```
type(scope): brief description

Detailed explanation of the change (if needed).

Fixes #issue_number
```

#### Commit Types

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks

#### Examples

```bash
feat(templates): add embedded system template

Add support for embedded C++ projects with:
- Microcontroller-specific build configurations
- Hardware abstraction layer templates
- Real-time operating system integration

Fixes #123

fix(cli): handle empty project names gracefully

Validate project names before processing to prevent
crashes when empty strings are provided.

Fixes #456
```

### 3. Code Organization

#### Adding New Templates

1. **Create template class**:

   ```cpp
   // src/templates/my_template.h
   #pragma once
   #include "template_base.h"

   class MyTemplate : public TemplateBase {
   public:
       MyTemplate(const CliOptions& options);
       bool create() override;

   protected:
       bool createProjectStructure() override;
       bool createBuildSystem() override;
       bool setupPackageManager() override;
       bool setupTestFramework() override;
   };
   ```

2. **Implement template**:

   ```cpp
   // src/templates/my_template.cpp
   #include "my_template.h"

   MyTemplate::MyTemplate(const CliOptions& options)
       : TemplateBase(options) {}

   bool MyTemplate::create() {
       return TemplateBase::create();
   }

   // Implement other methods...
   ```

3. **Register template**:

   ```cpp
   // In template_manager.cpp
   templateFactories_["my-template"] = [](const CliOptions& opts) {
       return std::make_unique<MyTemplate>(opts);
   };
   ```

4. **Add tests**:

   ```cpp
   // tests/test_my_template.cpp
   #include <gtest/gtest.h>
   #include "templates/my_template.h"

   TEST(MyTemplateTest, CreateBasicProject) {
       CliOptions options;
       options.projectName = "TestProject";
       options.templateType = TemplateType::MyTemplate;

       MyTemplate template(options);
       EXPECT_TRUE(template.create());
   }
   ```

## 🎨 Code Style

### C++ Style Guidelines

We follow a modified version of the Google C++ Style Guide:

#### Naming Conventions

```cpp
// Classes: PascalCase
class TemplateManager {};

// Functions and variables: camelCase
void createProject();
std::string projectName;

// Constants: UPPER_SNAKE_CASE
const int MAX_PROJECT_NAME_LENGTH = 255;

// Private members: trailing underscore
class MyClass {
private:
    std::string memberVariable_;
};

// Enums: PascalCase with scoped enums
enum class TemplateType {
    Console,
    Library,
    Gui
};
```

#### File Organization

```cpp
// Header file structure
#pragma once

// System includes
#include <string>
#include <vector>

// Third-party includes
#include <spdlog/spdlog.h>

// Project includes
#include "utils/file_utils.h"

// Forward declarations
class TemplateBase;

// Class definition
class MyClass {
public:
    // Public interface first

private:
    // Private members last
};
```

#### Code Formatting

Use clang-format with the provided configuration:

```bash
# Format all source files
find src/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Format specific file
clang-format -i src/templates/my_template.cpp
```

### CMake Style

```cmake
# Use lowercase for commands
cmake_minimum_required(VERSION 3.14)
project(cpp-scaffold)

# Use descriptive variable names
set(CORE_SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
)

# Proper indentation
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
```

## 🧪 Testing

### Writing Tests

#### Unit Tests

```cpp
#include <gtest/gtest.h>
#include "utils/string_utils.h"

class StringUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }

    void TearDown() override {
        // Cleanup code
    }
};

TEST_F(StringUtilsTest, ToLowerConvertsCorrectly) {
    EXPECT_EQ(utils::StringUtils::toLower("HELLO"), "hello");
    EXPECT_EQ(utils::StringUtils::toLower("MiXeD"), "mixed");
    EXPECT_EQ(utils::StringUtils::toLower(""), "");
}

TEST_F(StringUtilsTest, SplitHandlesDelimiters) {
    auto result = utils::StringUtils::split("a,b,c", ',');
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}
```

#### Integration Tests

```cpp
TEST(IntegrationTest, CreateAndBuildConsoleProject) {
    // Create temporary directory
    auto tempDir = std::filesystem::temp_directory_path() / "test_project";

    // Create project
    CliOptions options;
    options.projectName = "TestProject";
    options.templateType = TemplateType::Console;

    TemplateManager manager;
    ASSERT_TRUE(manager.createProject(options));

    // Verify project structure
    EXPECT_TRUE(std::filesystem::exists(tempDir / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(tempDir / "src" / "main.cpp"));

    // Cleanup
    std::filesystem::remove_all(tempDir);
}
```

### Running Tests

```bash
# Run all tests
cd build-dev
ctest

# Run specific test suite
ctest -R StringUtils

# Run tests with verbose output
ctest --verbose

# Run tests in parallel
ctest -j4
```

## 📚 Documentation

### Code Documentation

Use Doxygen-style comments for public APIs:

```cpp
/**
 * @brief Creates a new C++ project from the specified template
 *
 * This function creates a complete C++ project structure including
 * source files, build system configuration, and development tools.
 *
 * @param options Project configuration options
 * @return true if project creation succeeded, false otherwise
 *
 * @throws std::filesystem::filesystem_error if directory creation fails
 * @throws std::runtime_error if template is invalid
 *
 * @example
 * @code
 * CliOptions options;
 * options.projectName = "MyProject";
 * options.templateType = TemplateType::Console;
 *
 * TemplateManager manager;
 * if (manager.createProject(options)) {
 *     std::cout << "Project created successfully!" << std::endl;
 * }
 * @endcode
 */
bool createProject(const CliOptions& options);
```

### User Documentation

- Update relevant documentation files when adding features
- Include examples and use cases
- Provide clear, step-by-step instructions
- Test all documentation examples

## 🔄 Pull Request Process

### 1. Pre-submission Checklist

- [ ] Code follows style guidelines
- [ ] All tests pass
- [ ] New features include tests
- [ ] Documentation is updated
- [ ] Commit messages follow guidelines
- [ ] Branch is up to date with main

### 2. Submission Process

```bash
# Ensure your branch is up to date
git checkout main
git pull upstream main
git checkout your-feature-branch
git rebase main

# Run final checks
ninja
ctest
clang-format -i src/**/*.{cpp,h}

# Push to your fork
git push origin your-feature-branch
```

### 3. Pull Request Template

When creating a pull request, include:

```markdown
## Description

Brief description of changes

## Type of Change

- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing

- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Checklist

- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] Tests pass
```

### 4. Review Process

1. **Automated Checks**: CI/CD pipeline runs automatically
2. **Code Review**: Maintainers review code and provide feedback
3. **Discussion**: Address any questions or concerns
4. **Approval**: Once approved, maintainers will merge

## 🐛 Issue Reporting

### Bug Reports

Use the bug report template:

```markdown
**Bug Description**
Clear description of the bug

**Steps to Reproduce**

1. Step one
2. Step two
3. Step three

**Expected Behavior**
What should happen

**Actual Behavior**
What actually happens

**Environment**

- OS: [e.g., Ubuntu 20.04]
- Compiler: [e.g., GCC 9.3]
- CMake Version: [e.g., 3.18]
- CPP-Scaffold Version: [e.g., 1.0.0]

**Additional Context**
Any other relevant information
```

### Feature Requests

Use the feature request template:

```markdown
**Feature Description**
Clear description of the proposed feature

**Use Case**
Why is this feature needed?

**Proposed Solution**
How should this feature work?

**Alternatives Considered**
Other approaches you've considered

**Additional Context**
Any other relevant information
```

## 🏆 Recognition

Contributors will be recognized in:

- `CONTRIBUTORS.md` file
- Release notes
- Project documentation

Thank you for contributing to CPP-Scaffold! 🎉
