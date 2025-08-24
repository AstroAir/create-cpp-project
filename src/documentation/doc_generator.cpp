#include "doc_generator.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>
#include <fstream>

namespace documentation {

DocGenerator::DocGenerator(const DocConfig& config) : m_config(config) {
}

void DocGenerator::setConfig(const DocConfig& config) {
    m_config = config;
}

DocConfig DocGenerator::getConfig() const {
    return m_config;
}

bool DocGenerator::generateDocumentation(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Generating documentation for project: {}", projectPath.string());
        
        // Create output directory
        auto outputDir = projectPath / m_config.outputDirectory;
        if (!createOutputDirectory(outputDir)) {
            spdlog::error("Failed to create output directory: {}", outputDir.string());
            return false;
        }
        
        // Generate each document type
        for (const auto& docType : m_config.documentTypes) {
            if (!generateSpecificDoc(docType, projectPath)) {
                spdlog::error("Failed to generate documentation type: {}", static_cast<int>(docType));
                return false;
            }
        }
        
        // Generate index if requested
        if (m_config.generateIndex) {
            std::vector<std::string> files;
            for (const auto& entry : std::filesystem::directory_iterator(outputDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    files.push_back(entry.path().filename().string());
                }
            }
            
            std::string indexContent = generateIndex(files);
            auto indexPath = outputDir / "index.md";
            if (!utils::FileUtils::writeToFile(indexPath.string(), indexContent)) {
                spdlog::error("Failed to write index file");
                return false;
            }
        }
        
        spdlog::info("Documentation generation completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating documentation: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateSpecificDoc(DocType type, const std::filesystem::path& projectPath) {
    try {
        switch (type) {
            case DocType::README:
                return generateREADME(projectPath);
            case DocType::API:
                return generateAPIDocumentation(projectPath);
            case DocType::User:
                return generateUserGuide(projectPath);
            case DocType::Developer:
                return generateDeveloperGuide(projectPath);
            case DocType::Tutorial:
                return generateTutorials(projectPath);
            case DocType::Changelog:
                return generateChangelog(projectPath);
            case DocType::Contributing:
                return generateContributingGuide(projectPath);
            case DocType::License:
                return generateLicense(projectPath);
            default:
                spdlog::error("Unknown document type: {}", static_cast<int>(type));
                return false;
        }
    } catch (const std::exception& e) {
        spdlog::error("Error generating specific document: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateREADME(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateREADMEContent();
        content = replaceVariables(content);
        
        auto readmePath = projectPath / "README.md";
        if (!utils::FileUtils::writeToFile(readmePath.string(), content)) {
            spdlog::error("Failed to write README.md");
            return false;
        }
        
        spdlog::info("Generated README.md");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating README: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateAPIDocumentation(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Generating comprehensive API documentation...");

        APIDocExtractor extractor;
        auto sourceDir = projectPath / m_config.sourceDirectory;
        auto outputDir = projectPath / m_config.outputDirectory;

        // Create API documentation directory structure
        auto apiDir = outputDir / "api";
        if (!createOutputDirectory(apiDir)) {
            spdlog::error("Failed to create API documentation directory");
            return false;
        }

        // Extract API information from source code
        if (!extractor.extractFromDirectory(sourceDir)) {
            spdlog::error("Failed to extract API information from source directory");
            return false;
        }

        // Generate main API index
        std::string apiIndexContent = generateAPIIndexContent(extractor);
        apiIndexContent = replaceVariables(apiIndexContent);

        auto apiIndexPath = apiDir / "index.md";
        if (!utils::FileUtils::writeToFile(apiIndexPath.string(), apiIndexContent)) {
            spdlog::error("Failed to write API index documentation");
            return false;
        }

        // Generate detailed API documentation
        std::string apiContent = extractor.generateMarkdownAPI();
        apiContent = replaceVariables(apiContent);

        auto apiPath = apiDir / "reference.md";
        if (!utils::FileUtils::writeToFile(apiPath.string(), apiContent)) {
            spdlog::error("Failed to write API reference documentation");
            return false;
        }

        // Generate namespace-specific documentation
        auto namespaces = extractor.getNamespaces();
        for (const auto& ns : namespaces) {
            std::string nsContent = generateNamespaceDocumentation(ns);
            nsContent = replaceVariables(nsContent);

            auto nsPath = apiDir / (ns.name + ".md");
            if (!utils::FileUtils::writeToFile(nsPath.string(), nsContent)) {
                spdlog::warn("Failed to write namespace documentation for: {}", ns.name);
            }
        }

        // Generate class-specific documentation
        auto classes = extractor.getClasses();
        auto classesDir = apiDir / "classes";
        if (!createOutputDirectory(classesDir)) {
            spdlog::warn("Failed to create classes documentation directory");
        } else {
            for (const auto& cls : classes) {
                std::string classContent = generateClassDocumentation(cls);
                classContent = replaceVariables(classContent);

                auto classPath = classesDir / (cls.name + ".md");
                if (!utils::FileUtils::writeToFile(classPath.string(), classContent)) {
                    spdlog::warn("Failed to write class documentation for: {}", cls.name);
                }
            }
        }

        spdlog::info("Generated comprehensive API documentation with {} namespaces and {} classes",
                    namespaces.size(), classes.size());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error generating API documentation: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateUserGuide(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateUserGuideContent();
        content = replaceVariables(content);
        
        auto outputDir = projectPath / m_config.outputDirectory;
        auto userGuidePath = outputDir / "user-guide.md";
        
        if (!utils::FileUtils::writeToFile(userGuidePath.string(), content)) {
            spdlog::error("Failed to write user guide");
            return false;
        }
        
        spdlog::info("Generated user guide");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating user guide: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateDeveloperGuide(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateDeveloperGuideContent();
        content = replaceVariables(content);
        
        auto outputDir = projectPath / m_config.outputDirectory;
        auto devGuidePath = outputDir / "developer-guide.md";
        
        if (!utils::FileUtils::writeToFile(devGuidePath.string(), content)) {
            spdlog::error("Failed to write developer guide");
            return false;
        }
        
        spdlog::info("Generated developer guide");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating developer guide: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateTutorials(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateTutorialContent();
        content = replaceVariables(content);

        auto outputDir = projectPath / m_config.outputDirectory;
        auto tutorialsPath = outputDir / "tutorials.md";

        if (!utils::FileUtils::writeToFile(tutorialsPath.string(), content)) {
            spdlog::error("Failed to write tutorials");
            return false;
        }

        spdlog::info("Generated tutorials: {}", tutorialsPath.string());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error generating tutorials: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateChangelog(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateChangelogContent();
        content = replaceVariables(content);
        
        auto changelogPath = projectPath / "CHANGELOG.md";
        if (!utils::FileUtils::writeToFile(changelogPath.string(), content)) {
            spdlog::error("Failed to write CHANGELOG.md");
            return false;
        }
        
        spdlog::info("Generated CHANGELOG.md");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating changelog: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateContributingGuide(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateContributingContent();
        content = replaceVariables(content);
        
        auto contributingPath = projectPath / "CONTRIBUTING.md";
        if (!utils::FileUtils::writeToFile(contributingPath.string(), content)) {
            spdlog::error("Failed to write CONTRIBUTING.md");
            return false;
        }
        
        spdlog::info("Generated CONTRIBUTING.md");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating contributing guide: {}", e.what());
        return false;
    }
}

bool DocGenerator::generateLicense(const std::filesystem::path& projectPath) {
    try {
        std::string content = generateLicenseContent();
        content = replaceVariables(content);
        
        auto licensePath = projectPath / "LICENSE";
        if (!utils::FileUtils::writeToFile(licensePath.string(), content)) {
            spdlog::error("Failed to write LICENSE");
            return false;
        }
        
        spdlog::info("Generated LICENSE");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating license: {}", e.what());
        return false;
    }
}

std::string DocGenerator::generateREADMEContent() {
    std::ostringstream readme;

    // Header with badges
    readme << "# {{PROJECT_NAME}}\n\n";

    // Add badges based on configuration
    readme << "[![Build Status](https://github.com/{{GITHUB_USER}}/{{PROJECT_NAME}}/workflows/CI/badge.svg)](https://github.com/{{GITHUB_USER}}/{{PROJECT_NAME}}/actions)\n";
    readme << "[![License](https://img.shields.io/badge/license-{{LICENSE}}-blue.svg)](LICENSE)\n";
    readme << "[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2B-blue.svg)](https://en.cppreference.com/w/cpp/17)\n";
    readme << "[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](#)\n\n";

    readme << "{{PROJECT_DESCRIPTION}}\n\n";

    // Table of Contents
    readme << "## Table of Contents\n\n";
    readme << "- [Features](#features)\n";
    readme << "- [Quick Start](#quick-start)\n";
    readme << "- [Installation](#installation)\n";
    readme << "- [Usage](#usage)\n";
    readme << "- [Documentation](#documentation)\n";
    readme << "- [Examples](#examples)\n";
    readme << "- [Contributing](#contributing)\n";
    readme << "- [License](#license)\n";
    readme << "- [Support](#support)\n\n";

    // Enhanced Features section
    readme << "## Features\n\n";
    readme << "✨ **Key Features:**\n\n";
    readme << "- 🚀 High-performance C++ implementation\n";
    readme << "- 🔧 Modern C++17/20 features\n";
    readme << "- 📦 Easy integration with popular package managers\n";
    readme << "- 🧪 Comprehensive test suite\n";
    readme << "- 📚 Extensive documentation\n";
    readme << "- 🌐 Cross-platform compatibility\n\n";

    // Quick Start section
    readme << "## Quick Start\n\n";
    readme << "Get up and running in minutes:\n\n";
    readme << "```bash\n";
    readme << "# Clone the repository\n";
    readme << "git clone {{REPOSITORY}}\n";
    readme << "cd {{PROJECT_NAME}}\n\n";
    readme << "# Build with CMake\n";
    readme << "mkdir build && cd build\n";
    readme << "cmake .. -DCMAKE_BUILD_TYPE=Release\n";
    readme << "cmake --build . --parallel\n\n";
    readme << "# Run tests\n";
    readme << "ctest --output-on-failure\n";
    readme << "```\n\n";

    // Enhanced Installation section
    readme << "## Installation\n\n";
    readme << "### Prerequisites\n\n";
    readme << "| Requirement | Minimum Version | Recommended |\n";
    readme << "|-------------|-----------------|-------------|\n";
    readme << "| C++ Compiler | C++17 | C++20 |\n";
    readme << "| CMake | 3.15 | 3.20+ |\n";
    readme << "| Git | 2.0 | Latest |\n\n";

    readme << "### Package Managers\n\n";
    readme << "#### vcpkg\n";
    readme << "```bash\n";
    readme << "vcpkg install {{PROJECT_NAME}}\n";
    readme << "```\n\n";

    readme << "#### Conan\n";
    readme << "```bash\n";
    readme << "conan install {{PROJECT_NAME}}/{{PROJECT_VERSION}}@\n";
    readme << "```\n\n";

    readme << "### Building from Source\n\n";
    readme << "#### Linux/macOS\n";
    readme << "```bash\n";
    readme << "git clone {{REPOSITORY}}\n";
    readme << "cd {{PROJECT_NAME}}\n";
    readme << "mkdir build && cd build\n";
    readme << "cmake .. -DCMAKE_BUILD_TYPE=Release\n";
    readme << "make -j$(nproc)\n";
    readme << "sudo make install\n";
    readme << "```\n\n";

    readme << "#### Windows\n";
    readme << "```cmd\n";
    readme << "git clone {{REPOSITORY}}\n";
    readme << "cd {{PROJECT_NAME}}\n";
    readme << "mkdir build && cd build\n";
    readme << "cmake .. -G \"Visual Studio 16 2019\" -A x64\n";
    readme << "cmake --build . --config Release\n";
    readme << "cmake --install .\n";
    readme << "```\n\n";

    // Enhanced Usage section
    readme << "## Usage\n\n";
    readme << "### Basic Example\n\n";
    readme << "```cpp\n";
    readme << "#include <{{PROJECT_NAME}}/{{PROJECT_NAME}}.h>\n";
    readme << "#include <iostream>\n\n";
    readme << "int main() {\n";
    readme << "    {{PROJECT_NAME}}::Example example;\n";
    readme << "    \n";
    readme << "    // Initialize and use the library\n";
    readme << "    if (example.initialize()) {\n";
    readme << "        std::cout << \"{{PROJECT_NAME}} initialized successfully!\" << std::endl;\n";
    readme << "        example.run();\n";
    readme << "    }\n";
    readme << "    \n";
    readme << "    return 0;\n";
    readme << "}\n";
    readme << "```\n\n";

    readme << "### CMake Integration\n\n";
    readme << "```cmake\n";
    readme << "find_package({{PROJECT_NAME}} REQUIRED)\n";
    readme << "target_link_libraries(your_target {{PROJECT_NAME}}::{{PROJECT_NAME}})\n";
    readme << "```\n\n";

    // Documentation section
    readme << "## Documentation\n\n";
    readme << "📖 **Comprehensive Documentation:**\n\n";
    readme << "- 📋 [API Reference](docs/api/index.html) - Complete API documentation\n";
    readme << "- 👥 [User Guide](docs/user-guide.md) - Step-by-step usage guide\n";
    readme << "- 🔧 [Developer Guide](docs/developer-guide.md) - Contributing and development\n";
    readme << "- 🎯 [Tutorials](docs/tutorials/) - Hands-on learning materials\n";
    readme << "- ❓ [FAQ](docs/faq.md) - Frequently asked questions\n\n";

    // Examples section
    readme << "## Examples\n\n";
    readme << "Explore our [examples directory](examples/) for:\n\n";
    readme << "- 🎯 [Basic Usage](examples/basic/) - Simple getting started examples\n";
    readme << "- 🚀 [Advanced Features](examples/advanced/) - Complex use cases\n";
    readme << "- 🔌 [Integrations](examples/integrations/) - Third-party library integration\n";
    readme << "- 🧪 [Benchmarks](examples/benchmarks/) - Performance demonstrations\n\n";

    // Contributing section
    readme << "## Contributing\n\n";
    readme << "We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.\n\n";
    readme << "### Development Setup\n\n";
    readme << "```bash\n";
    readme << "# Fork and clone the repository\n";
    readme << "git clone https://github.com/YOUR_USERNAME/{{PROJECT_NAME}}.git\n";
    readme << "cd {{PROJECT_NAME}}\n\n";
    readme << "# Install development dependencies\n";
    readme << "pip install -r requirements-dev.txt\n\n";
    readme << "# Run tests\n";
    readme << "mkdir build && cd build\n";
    readme << "cmake .. -DBUILD_TESTING=ON\n";
    readme << "make && ctest\n";
    readme << "```\n\n";

    // License section
    readme << "## License\n\n";
    readme << "This project is licensed under the {{LICENSE}} License - see the [LICENSE](LICENSE) file for details.\n\n";

    // Support section
    readme << "## Support\n\n";
    readme << "- 🐛 [Report Issues]({{REPOSITORY}}/issues) - Bug reports and feature requests\n";
    readme << "- 💬 [Discussions]({{REPOSITORY}}/discussions) - Community discussions\n";
    readme << "- 📧 [Email](mailto:{{EMAIL}}) - Direct contact\n";
    readme << "- 📖 [Documentation](docs/) - Comprehensive guides\n\n";

    // Footer
    readme << "---\n\n";
    readme << "**Made with ❤️ by [{{AUTHOR}}](mailto:{{EMAIL}})**\n\n";
    readme << "*Generated by [CPP-Scaffold](https://github.com/cpp-scaffold/cpp-scaffold)*\n";

    return readme.str();
}

std::string DocGenerator::generateUserGuideContent() {
    std::ostringstream guide;
    
    guide << "# {{PROJECT_NAME}} User Guide\n\n";
    guide << "Welcome to the {{PROJECT_NAME}} user guide. This document will help you get started with using {{PROJECT_NAME}}.\n\n";
    
    guide << "## Table of Contents\n\n";
    guide << "1. [Getting Started](#getting-started)\n";
    guide << "2. [Basic Usage](#basic-usage)\n";
    guide << "3. [Advanced Features](#advanced-features)\n";
    guide << "4. [Configuration](#configuration)\n";
    guide << "5. [Troubleshooting](#troubleshooting)\n\n";
    
    guide << "## Getting Started\n\n";
    guide << "This section covers the basic setup and initial configuration of {{PROJECT_NAME}}.\n\n";
    
    guide << "### Installation\n\n";
    guide << "Follow the installation instructions in the [README](../README.md).\n\n";
    
    guide << "### First Steps\n\n";
    guide << "1. Step 1\n";
    guide << "2. Step 2\n";
    guide << "3. Step 3\n\n";
    
    guide << "## Basic Usage\n\n";
    guide << "This section covers the basic usage patterns of {{PROJECT_NAME}}.\n\n";
    
    guide << "### Example 1\n\n";
    guide << "```cpp\n";
    guide << "// Basic example\n";
    guide << "```\n\n";
    
    guide << "### Example 2\n\n";
    guide << "```cpp\n";
    guide << "// Another example\n";
    guide << "```\n\n";
    
    guide << "## Advanced Features\n\n";
    guide << "This section covers advanced features and use cases.\n\n";
    
    guide << "## Configuration\n\n";
    guide << "This section covers configuration options.\n\n";
    
    guide << "## Troubleshooting\n\n";
    guide << "Common issues and their solutions:\n\n";
    guide << "### Issue 1\n\n";
    guide << "**Problem:** Description of the problem.\n\n";
    guide << "**Solution:** Description of the solution.\n\n";
    
    return guide.str();
}

std::string DocGenerator::generateDeveloperGuideContent() {
    std::ostringstream guide;
    
    guide << "# {{PROJECT_NAME}} Developer Guide\n\n";
    guide << "This guide is for developers who want to contribute to {{PROJECT_NAME}} or understand its internals.\n\n";
    
    guide << "## Table of Contents\n\n";
    guide << "1. [Development Setup](#development-setup)\n";
    guide << "2. [Architecture](#architecture)\n";
    guide << "3. [Building](#building)\n";
    guide << "4. [Testing](#testing)\n";
    guide << "5. [Contributing](#contributing)\n\n";
    
    guide << "## Development Setup\n\n";
    guide << "### Prerequisites\n\n";
    guide << "- C++17 or later\n";
    guide << "- CMake 3.10 or later\n";
    guide << "- Git\n\n";
    
    guide << "### Setting up the Development Environment\n\n";
    guide << "1. Clone the repository\n";
    guide << "2. Install dependencies\n";
    guide << "3. Configure your IDE\n\n";
    
    guide << "## Architecture\n\n";
    guide << "{{PROJECT_NAME}} follows a modular architecture with the following components:\n\n";
    guide << "- **Core**: Main functionality\n";
    guide << "- **Utils**: Utility functions\n";
    guide << "- **Config**: Configuration management\n\n";
    
    guide << "## Building\n\n";
    guide << "### Debug Build\n\n";
    guide << "```bash\n";
    guide << "mkdir build-debug && cd build-debug\n";
    guide << "cmake -DCMAKE_BUILD_TYPE=Debug ..\n";
    guide << "cmake --build .\n";
    guide << "```\n\n";
    
    guide << "### Release Build\n\n";
    guide << "```bash\n";
    guide << "mkdir build-release && cd build-release\n";
    guide << "cmake -DCMAKE_BUILD_TYPE=Release ..\n";
    guide << "cmake --build .\n";
    guide << "```\n\n";
    
    guide << "## Testing\n\n";
    guide << "### Running Tests\n\n";
    guide << "```bash\n";
    guide << "cd build\n";
    guide << "ctest\n";
    guide << "```\n\n";
    
    guide << "### Writing Tests\n\n";
    guide << "Follow the existing test patterns in the `tests/` directory.\n\n";
    
    guide << "## Contributing\n\n";
    guide << "Please read [CONTRIBUTING.md](../CONTRIBUTING.md) for detailed contribution guidelines.\n";
    
    return guide.str();
}

std::string DocGenerator::generateTutorialContent() {
    std::ostringstream tutorial;

    tutorial << "# " << m_config.projectName << " Tutorials\n\n";
    tutorial << "This document contains step-by-step tutorials to help you get started with " << m_config.projectName << ".\n\n";

    tutorial << "## Table of Contents\n\n";
    tutorial << "1. [Getting Started](#getting-started)\n";
    tutorial << "2. [Basic Usage](#basic-usage)\n";
    tutorial << "3. [Advanced Features](#advanced-features)\n";
    tutorial << "4. [Examples](#examples)\n";
    tutorial << "5. [Troubleshooting](#troubleshooting)\n\n";

    tutorial << "## Getting Started\n\n";
    tutorial << "### Prerequisites\n\n";
    tutorial << "Before you begin, ensure you have the following installed:\n\n";
    tutorial << "- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)\n";
    tutorial << "- CMake 3.15 or higher\n";
    tutorial << "- Git\n\n";

    tutorial << "### Installation\n\n";
    tutorial << "1. Clone the repository:\n";
    tutorial << "   ```bash\n";
    tutorial << "   git clone <repository-url>\n";
    tutorial << "   cd " << m_config.projectName << "\n";
    tutorial << "   ```\n\n";

    tutorial << "2. Build the project:\n";
    tutorial << "   ```bash\n";
    tutorial << "   mkdir build && cd build\n";
    tutorial << "   cmake ..\n";
    tutorial << "   cmake --build .\n";
    tutorial << "   ```\n\n";

    tutorial << "## Basic Usage\n\n";
    tutorial << "### Your First Program\n\n";
    tutorial << "Here's a simple example to get you started:\n\n";
    tutorial << "```cpp\n";
    tutorial << "#include <iostream>\n";
    tutorial << "#include \"" << m_config.projectName << ".h\"\n\n";
    tutorial << "int main() {\n";
    tutorial << "    std::cout << \"Hello from " << m_config.projectName << "!\" << std::endl;\n";
    tutorial << "    return 0;\n";
    tutorial << "}\n";
    tutorial << "```\n\n";

    tutorial << "### Running Tests\n\n";
    tutorial << "To run the test suite:\n\n";
    tutorial << "```bash\n";
    tutorial << "cd build\n";
    tutorial << "ctest\n";
    tutorial << "```\n\n";

    tutorial << "## Advanced Features\n\n";
    tutorial << "### Configuration\n\n";
    tutorial << "The project can be configured using various options. See the [Developer Guide](developer-guide.md) for detailed configuration options.\n\n";

    tutorial << "### Custom Extensions\n\n";
    tutorial << "You can extend the functionality by implementing custom plugins. Refer to the API documentation for details.\n\n";

    tutorial << "## Examples\n\n";
    tutorial << "Check the `examples/` directory for complete working examples:\n\n";
    tutorial << "- `examples/basic/` - Basic usage examples\n";
    tutorial << "- `examples/advanced/` - Advanced feature demonstrations\n";
    tutorial << "- `examples/integration/` - Integration with other libraries\n\n";

    tutorial << "## Troubleshooting\n\n";
    tutorial << "### Common Issues\n\n";
    tutorial << "**Build fails with compiler errors:**\n";
    tutorial << "- Ensure you're using a C++17 compatible compiler\n";
    tutorial << "- Check that all dependencies are installed\n\n";

    tutorial << "**Tests fail:**\n";
    tutorial << "- Make sure the project was built in Debug mode for testing\n";
    tutorial << "- Check that all required test data files are present\n\n";

    tutorial << "**Runtime errors:**\n";
    tutorial << "- Verify that all shared libraries are in the system PATH\n";
    tutorial << "- Check the log files for detailed error messages\n\n";

    tutorial << "### Getting Help\n\n";
    tutorial << "If you encounter issues not covered here:\n\n";
    tutorial << "1. Check the [FAQ](faq.md)\n";
    tutorial << "2. Search existing [issues](issues-url)\n";
    tutorial << "3. Create a new issue with detailed information\n\n";

    tutorial << "## Next Steps\n\n";
    tutorial << "- Read the [User Guide](user-guide.md) for comprehensive usage information\n";
    tutorial << "- Explore the [API Documentation](api.md) for detailed reference\n";
    tutorial << "- Check out the [Developer Guide](developer-guide.md) if you want to contribute\n";

    return tutorial.str();
}

std::string DocGenerator::generateChangelogContent() {
    std::ostringstream changelog;
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    changelog << "# Changelog\n\n";
    changelog << "All notable changes to this project will be documented in this file.\n\n";
    changelog << "The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),\n";
    changelog << "and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).\n\n";
    
    changelog << "## [Unreleased]\n\n";
    changelog << "### Added\n";
    changelog << "- Initial project structure\n\n";
    
    changelog << "## [{{PROJECT_VERSION}}] - " << std::put_time(&tm, "%Y-%m-%d") << "\n\n";
    changelog << "### Added\n";
    changelog << "- Initial release\n";
    changelog << "- Basic functionality\n";
    changelog << "- Documentation\n\n";
    
    return changelog.str();
}

std::string DocGenerator::generateContributingContent() {
    std::ostringstream contributing;
    
    contributing << "# Contributing to {{PROJECT_NAME}}\n\n";
    contributing << "First off, thank you for considering contributing to {{PROJECT_NAME}}! It's people like you that make {{PROJECT_NAME}} such a great tool.\n\n";
    
    contributing << "## Code of Conduct\n\n";
    contributing << "This project and everyone participating in it is governed by our Code of Conduct. By participating, you are expected to uphold this code.\n\n";
    
    contributing << "## How Can I Contribute?\n\n";
    contributing << "### Reporting Bugs\n\n";
    contributing << "Before creating bug reports, please check the existing issues as you might find out that you don't need to create one.\n\n";
    
    contributing << "#### How Do I Submit A (Good) Bug Report?\n\n";
    contributing << "Bugs are tracked as GitHub issues. Create an issue and provide the following information:\n\n";
    contributing << "- Use a clear and descriptive title\n";
    contributing << "- Describe the exact steps which reproduce the problem\n";
    contributing << "- Provide specific examples to demonstrate the steps\n";
    contributing << "- Describe the behavior you observed after following the steps\n";
    contributing << "- Explain which behavior you expected to see instead and why\n\n";
    
    contributing << "### Suggesting Enhancements\n\n";
    contributing << "Enhancement suggestions are tracked as GitHub issues. Create an issue and provide the following information:\n\n";
    contributing << "- Use a clear and descriptive title\n";
    contributing << "- Provide a step-by-step description of the suggested enhancement\n";
    contributing << "- Provide specific examples to demonstrate the steps\n";
    contributing << "- Describe the current behavior and explain which behavior you expected to see instead\n\n";
    
    contributing << "### Pull Requests\n\n";
    contributing << "1. Fork the repo and create your branch from `main`\n";
    contributing << "2. If you've added code that should be tested, add tests\n";
    contributing << "3. If you've changed APIs, update the documentation\n";
    contributing << "4. Ensure the test suite passes\n";
    contributing << "5. Make sure your code lints\n";
    contributing << "6. Issue that pull request!\n\n";
    
    contributing << "## Development Process\n\n";
    contributing << "### Setting up your development environment\n\n";
    contributing << "1. Fork the repository\n";
    contributing << "2. Clone your fork\n";
    contributing << "3. Install dependencies\n";
    contributing << "4. Create a branch for your changes\n\n";
    
    contributing << "### Coding Standards\n\n";
    contributing << "- Follow the existing code style\n";
    contributing << "- Write meaningful commit messages\n";
    contributing << "- Add tests for new functionality\n";
    contributing << "- Update documentation as needed\n\n";
    
    contributing << "## License\n\n";
    contributing << "By contributing, you agree that your contributions will be licensed under the {{LICENSE}} License.\n";
    
    return contributing.str();
}

std::string DocGenerator::generateLicenseContent() {
    if (m_config.license == "MIT") {
        return doc_utils::getLicenseText("MIT");
    } else if (m_config.license == "Apache-2.0") {
        return doc_utils::getLicenseText("Apache-2.0");
    } else if (m_config.license == "GPL-3.0") {
        return doc_utils::getLicenseText("GPL-3.0");
    } else {
        return doc_utils::getLicenseText("MIT"); // Default to MIT
    }
}

std::string DocGenerator::replaceVariables(const std::string& content) const {
    std::string result = content;
    
    // Replace project variables
    result = utils::StringUtils::replace(result, "{{PROJECT_NAME}}", m_config.projectName);
    result = utils::StringUtils::replace(result, "{{PROJECT_VERSION}}", m_config.projectVersion);
    result = utils::StringUtils::replace(result, "{{PROJECT_DESCRIPTION}}", m_config.projectDescription);
    result = utils::StringUtils::replace(result, "{{AUTHOR}}", m_config.author);
    result = utils::StringUtils::replace(result, "{{EMAIL}}", m_config.email);
    result = utils::StringUtils::replace(result, "{{LICENSE}}", m_config.license);
    result = utils::StringUtils::replace(result, "{{REPOSITORY}}", m_config.repository);
    result = utils::StringUtils::replace(result, "{{WEBSITE}}", m_config.website);
    
    // Replace current date
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    char dateBuffer[32];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &tm);
    result = utils::StringUtils::replace(result, "{{CURRENT_DATE}}", std::string(dateBuffer));
    
    std::strftime(dateBuffer, sizeof(dateBuffer), "%Y", &tm);
    result = utils::StringUtils::replace(result, "{{CURRENT_YEAR}}", std::string(dateBuffer));
    
    return result;
}

std::string DocGenerator::generateIndex(const std::vector<std::string>& files) const {
    std::ostringstream index;
    
    index << "# {{PROJECT_NAME}} Documentation\n\n";
    index << "Welcome to the {{PROJECT_NAME}} documentation.\n\n";
    
    index << "## Available Documents\n\n";
    for (const auto& file : files) {
        if (file != "index.md") {
            std::string title = file;
            title = utils::StringUtils::replace(title, ".md", "");
            title = utils::StringUtils::replace(title, "-", " ");
            title = utils::StringUtils::replace(title, "_", " ");
            
            // Capitalize first letter of each word
            std::istringstream iss(title);
            std::string word;
            std::ostringstream titleStream;
            bool first = true;
            
            while (iss >> word) {
                if (!first) titleStream << " ";
                word[0] = std::toupper(word[0]);
                titleStream << word;
                first = false;
            }
            
            index << "- [" << titleStream.str() << "](" << file << ")\n";
        }
    }
    
    index << "\n";
    return index.str();
}

bool DocGenerator::createOutputDirectory(const std::filesystem::path& path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error creating output directory: {}", e.what());
        return false;
    }
}

// Documentation utilities implementation
namespace doc_utils {
    std::string getLicenseText(const std::string& licenseName) {
        if (licenseName == "MIT") {
            return R"(MIT License

Copyright (c) {{CURRENT_YEAR}} {{AUTHOR}}

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)";
        } else if (licenseName == "Apache-2.0") {
            return R"(Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/

TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

1. Definitions.

"License" shall mean the terms and conditions for use, reproduction,
and distribution as defined by Sections 1 through 9 of this document.

"Licensor" shall mean the copyright owner or entity granting the License.

"Legal Entity" shall mean the union of the acting entity and all
other entities that control, are controlled by, or are under common
control with that entity. For the purposes of this definition,
"control" means (i) the power, direct or indirect, to cause the
direction or management of such entity, whether by contract or
otherwise, or (ii) ownership of fifty percent (50%) or more of the
outstanding shares, or (iii) beneficial ownership of such entity.

"You" (or "Your") shall mean an individual or Legal Entity
exercising permissions granted by this License.

"Source" form shall mean the preferred form for making modifications,
including but not limited to software source code, documentation
source, and configuration files.

"Object" form shall mean any form resulting from mechanical
transformation or translation of a Source form, including but
not limited to compiled object code, generated documentation,
and conversions to other media types.

"Work" shall mean the work of authorship, whether in Source or
Object form, made available under the License, as indicated by a
copyright notice that is included in or attached to the work
(which shall not include communication that is conspicuously
marked or otherwise designated in writing by the copyright owner
as "Not a Contribution").

"Derivative Works" shall mean any work, whether in Source or Object
form, that is based upon (or derived from) the Work and for which the
editorial revisions, annotations, elaborations, or other modifications
represent, as a whole, an original work of authorship. For the purposes
of this License, Derivative Works shall not include works that remain
separable from, or merely link (or bind by name) to the interfaces of,
the Work and derivative works thereof.

"Contribution" shall mean any work of authorship, including
the original version of the Work and any modifications or additions
to that Work or Derivative Works thereof, that is intentionally
submitted to Licensor for inclusion in the Work by the copyright owner
or by an individual or Legal Entity authorized to submit on behalf of
the copyright owner. For the purposes of this definition, "submitted"
means any form of electronic, verbal, or written communication sent
to the Licensor or its representatives, including but not limited to
communication on electronic mailing lists, source code control
systems, and issue tracking systems that are managed by, or on behalf
of, the Licensor for the purpose of discussing and improving the Work,
but excluding communication that is conspicuously marked or otherwise
designated in writing by the copyright owner as "Not a Contribution".

2. Grant of Copyright License. Subject to the terms and conditions of
this License, each Contributor hereby grants to You a perpetual,
worldwide, non-exclusive, no-charge, royalty-free, irrevocable
copyright license to use, reproduce, modify, distribute, and prepare
Derivative Works of, publicly display, publicly perform, sublicense,
and distribute the Work and such Derivative Works in Source or Object
form.

3. Grant of Patent License. Subject to the terms and conditions of
this License, each Contributor hereby grants to You a perpetual,
worldwide, non-exclusive, no-charge, royalty-free, irrevocable
(except as stated in this section) patent license to make, have made,
use, offer to sell, sell, import, and otherwise transfer the Work,
where such license applies only to those patent claims licensable
by such Contributor that are necessarily infringed by their
Contribution(s) alone or by combination of their Contribution(s)
with the Work to which such Contribution(s) was submitted. If You
institute patent litigation against any entity (including a
cross-claim or counterclaim in a lawsuit) alleging that the Work
or a Contribution incorporated within the Work constitutes direct
or contributory patent infringement, then any patent licenses
granted to You under this License for that Work shall terminate
as of the date such litigation is filed.

4. Redistribution. You may reproduce and distribute copies of the
Work or Derivative Works thereof in any medium, with or without
modifications, and in Source or Object form, provided that You
meet the following conditions:

(a) You must give any other recipients of the Work or
    Derivative Works a copy of this License; and

(b) You must cause any modified files to carry prominent notices
    stating that You changed the files; and

(c) You must retain, in the Source form of any Derivative Works
    that You distribute, all copyright, trademark, patent,
    attribution and other notices from the Source form of the Work,
    excluding those notices that do not pertain to any part of
    the Derivative Works; and

(d) If the Work includes a "NOTICE" text file as part of its
    distribution, then any Derivative Works that You distribute must
    include a readable copy of the attribution notices contained
    within such NOTICE file, excluding those notices that do not
    pertain to any part of the Derivative Works, in at least one
    of the following places: within a NOTICE text file distributed
    as part of the Derivative Works; within the Source form or
    documentation, if provided along with the Derivative Works; or,
    within a display generated by the Derivative Works, if and
    wherever such third-party notices normally appear. The contents
    of the NOTICE file are for informational purposes only and
    do not modify the License. You may add Your own attribution
    notices within Derivative Works that You distribute, alongside
    or as an addendum to the NOTICE text from the Work, provided
    that such additional attribution notices cannot be construed
    as modifying the License.

You may add Your own copyright notice to Your modifications and
may provide additional or different license terms and conditions
for use, reproduction, or distribution of Your modifications, or
for any such Derivative Works as a whole, provided Your use,
reproduction, and distribution of the Work otherwise complies with
the conditions stated in this License.

5. Submission of Contributions. Unless You explicitly state otherwise,
any Contribution intentionally submitted for inclusion in the Work
by You to the Licensor shall be under the terms and conditions of
this License, without any additional terms or conditions.
Notwithstanding the above, nothing herein shall supersede or modify
the terms of any separate license agreement you may have executed
with Licensor regarding such Contributions.

6. Trademarks. This License does not grant permission to use the trade
names, trademarks, service marks, or product names of the Licensor,
except as required for reasonable and customary use in describing the
origin of the Work and reproducing the content of the NOTICE file.

7. Disclaimer of Warranty. Unless required by applicable law or
agreed to in writing, Licensor provides the Work (and each
Contributor provides its Contributions) on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied, including, without limitation, any warranties or conditions
of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
PARTICULAR PURPOSE. You are solely responsible for determining the
appropriateness of using or redistributing the Work and assume any
risks associated with Your exercise of permissions under this License.

8. Limitation of Liability. In no event and under no legal theory,
whether in tort (including negligence), contract, or otherwise,
unless required by applicable law (such as deliberate and grossly
negligent acts) or agreed to in writing, shall any Contributor be
liable to You for damages, including any direct, indirect, special,
incidental, or consequential damages of any character arising as a
result of this License or out of the use or inability to use the
Work (including but not limited to damages for loss of goodwill,
work stoppage, computer failure or malfunction, or any and all
other commercial damages or losses), even if such Contributor
has been advised of the possibility of such damages.

9. Accepting Warranty or Additional Liability. When redistributing
the Work or Derivative Works thereof, You may choose to offer,
and charge a fee for, acceptance of support, warranty, indemnity,
or other liability obligations and/or rights consistent with this
License. However, in accepting such obligations, You may act only
on Your own behalf and on Your sole responsibility, not on behalf
of any other Contributor, and only if You agree to indemnify,
defend, and hold each Contributor harmless for any liability
incurred by, or claims asserted against, such Contributor by reason
of your accepting any such warranty or additional liability.

END OF TERMS AND CONDITIONS

APPENDIX: How to apply the Apache License to your work.

To apply the Apache License to your work, attach the following
boilerplate notice, with the fields enclosed by brackets "[]"
replaced with your own identifying information. (Don't include
the brackets!)  The text should be enclosed in the appropriate
comment syntax for the file format. We also recommend that a
file or class name and description of purpose be included on the
same "printed page" as the copyright notice for easier
identification within third-party archives.

Copyright {{CURRENT_YEAR}} {{AUTHOR}}

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.)";
        } else {
            // Default to MIT
            return getLicenseText("MIT");
        }
    }

    std::vector<std::string> getAvailableLicenses() {
        return {"MIT", "Apache-2.0", "GPL-3.0", "BSD-3-Clause", "BSD-2-Clause", "LGPL-2.1", "LGPL-3.0"};
    }

    bool isValidLicense(const std::string& licenseName) {
        auto licenses = getAvailableLicenses();
        return std::find(licenses.begin(), licenses.end(), licenseName) != licenses.end();
    }

    std::string formatToString(DocFormat format) {
        switch (format) {
            case DocFormat::Markdown: return "Markdown";
            case DocFormat::HTML: return "HTML";
            case DocFormat::PDF: return "PDF";
            case DocFormat::LaTeX: return "LaTeX";
            case DocFormat::Sphinx: return "Sphinx";
            case DocFormat::GitBook: return "GitBook";
            case DocFormat::Doxygen: return "Doxygen";
            case DocFormat::Custom: return "Custom";
            default: return "Unknown";
        }
    }

    std::optional<DocFormat> stringToFormat(const std::string& name) {
        std::string lowerName = utils::StringUtils::toLower(name);

        if (lowerName == "markdown" || lowerName == "md") {
            return DocFormat::Markdown;
        } else if (lowerName == "html") {
            return DocFormat::HTML;
        } else if (lowerName == "pdf") {
            return DocFormat::PDF;
        } else if (lowerName == "latex" || lowerName == "tex") {
            return DocFormat::LaTeX;
        } else if (lowerName == "sphinx") {
            return DocFormat::Sphinx;
        } else if (lowerName == "gitbook") {
            return DocFormat::GitBook;
        } else if (lowerName == "doxygen") {
            return DocFormat::Doxygen;
        } else if (lowerName == "custom") {
            return DocFormat::Custom;
        }

        return std::nullopt;
    }

    std::string sanitizeFilename(const std::string& filename) {
        std::string result = filename;

        // Replace invalid characters
        std::regex invalidChars(R"([<>:"/\\|?*])");
        result = std::regex_replace(result, invalidChars, "_");

        // Remove leading/trailing spaces and dots
        result = utils::StringUtils::trim(result);
        while (!result.empty() && result.back() == '.') {
            result.pop_back();
        }

        return result;
    }

    bool isDocumentationFile(const std::filesystem::path& filePath) {
        std::string extension = filePath.extension().string();
        std::string lowerExt = utils::StringUtils::toLower(extension);

        return lowerExt == ".md" || lowerExt == ".markdown" ||
               lowerExt == ".rst" || lowerExt == ".txt" ||
               lowerExt == ".html" || lowerExt == ".htm";
    }

    std::vector<std::filesystem::path> findDocumentationFiles(const std::filesystem::path& directory) {
        std::vector<std::filesystem::path> docFiles;

        if (!std::filesystem::exists(directory)) {
            return docFiles;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && isDocumentationFile(entry.path())) {
                docFiles.push_back(entry.path());
            }
        }

        return docFiles;
    }
}

// APIDocExtractor implementation
APIDocExtractor::APIDocExtractor() {
    // Initialize empty containers
}

bool APIDocExtractor::extractFromDirectory(const std::filesystem::path& sourceDir) {
    try {
        if (!std::filesystem::exists(sourceDir) || !std::filesystem::is_directory(sourceDir)) {
            spdlog::error("Source directory does not exist: {}", sourceDir.string());
            return false;
        }

        // Clear existing data
        m_namespaces.clear();
        m_classes.clear();
        m_functions.clear();

        // Recursively process all header files
        for (const auto& entry : std::filesystem::recursive_directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                auto extension = entry.path().extension().string();
                if (extension == ".h" || extension == ".hpp" || extension == ".hxx") {
                    if (!extractFromFile(entry.path())) {
                        spdlog::warn("Failed to extract from file: {}", entry.path().string());
                    }
                }
            }
        }

        spdlog::info("Extracted API documentation from {} files", m_classes.size() + m_functions.size());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error extracting from directory: {}", e.what());
        return false;
    }
}

bool APIDocExtractor::extractFromFile(const std::filesystem::path& sourceFile) {
    try {
        std::ifstream file(sourceFile);
        if (!file.is_open()) {
            return false;
        }

        // Helper lambda for C++17 compatibility
        auto starts_with = [](const std::string& str, const std::string& prefix) {
            return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
        };

        std::string line;
        std::string currentNamespace;
        bool inClass = false;
        ClassInfo currentClass;

        while (std::getline(file, line)) {
            // Simple parsing - this is a basic implementation
            // In a real implementation, you'd use a proper C++ parser

            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Skip empty lines and comments
            if (line.empty() || starts_with(line, "//") || starts_with(line, "/*")) {
                continue;
            }

            // Extract namespace
            if (starts_with(line, "namespace ")) {
                auto pos = line.find(' ');
                if (pos != std::string::npos) {
                    currentNamespace = line.substr(pos + 1);
                    // Remove any trailing characters like {
                    auto bracePos = currentNamespace.find('{');
                    if (bracePos != std::string::npos) {
                        currentNamespace = currentNamespace.substr(0, bracePos);
                    }
                    // Trim again
                    currentNamespace.erase(0, currentNamespace.find_first_not_of(" \t"));
                    currentNamespace.erase(currentNamespace.find_last_not_of(" \t") + 1);
                }
            }

            // Extract class/struct
            if (starts_with(line, "class ") || starts_with(line, "struct ")) {
                inClass = true;
                currentClass = ClassInfo{};
                currentClass.namespace_ = currentNamespace;

                auto pos = line.find(' ');
                if (pos != std::string::npos) {
                    std::string rest = line.substr(pos + 1);
                    auto spacePos = rest.find(' ');
                    auto bracePos = rest.find('{');
                    auto colonPos = rest.find(':');

                    size_t endPos = std::min({spacePos, bracePos, colonPos});
                    if (endPos == std::string::npos) {
                        endPos = rest.length();
                    }

                    currentClass.name = rest.substr(0, endPos);
                    currentClass.description = "Class " + currentClass.name;
                }
            }

            // End of class
            if (inClass && line.find("};") != std::string::npos) {
                m_classes.push_back(currentClass);
                inClass = false;
            }

            // Extract functions (basic detection)
            if (line.find('(') != std::string::npos && line.find(')') != std::string::npos &&
                !starts_with(line, "#") && line.find("if") != 0 && line.find("for") != 0 &&
                line.find("while") != 0) {

                // Very basic function name extraction
                auto parenPos = line.find('(');
                std::string beforeParen = line.substr(0, parenPos);
                auto lastSpace = beforeParen.find_last_of(" \t");
                std::string funcName;
                if (lastSpace != std::string::npos) {
                    funcName = beforeParen.substr(lastSpace + 1);
                } else {
                    funcName = beforeParen;
                }

                if (inClass) {
                    currentClass.methods.push_back(funcName);
                } else {
                    FunctionInfo func;
                    func.namespace_ = currentNamespace;
                    func.name = funcName;
                    func.description = "Function " + funcName;
                    func.returnType = "auto"; // Simplified
                    m_functions.push_back(func);
                }
            }
        }

        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error extracting from file {}: {}", sourceFile.string(), e.what());
        return false;
    }
}

std::string APIDocExtractor::generateMarkdownAPI() const {
    std::ostringstream api;

    api << "# API Documentation\n\n";
    api << "This document provides a comprehensive reference for the API.\n\n";

    // Table of contents
    api << "## Table of Contents\n\n";
    if (!m_namespaces.empty()) {
        api << "- [Namespaces](#namespaces)\n";
    }
    if (!m_classes.empty()) {
        api << "- [Classes](#classes)\n";
    }
    if (!m_functions.empty()) {
        api << "- [Functions](#functions)\n";
    }
    api << "\n";

    // Namespaces
    if (!m_namespaces.empty()) {
        api << "## Namespaces\n\n";
        for (const auto& ns : m_namespaces) {
            api << "### " << ns.name << "\n\n";
            api << ns.description << "\n\n";
        }
    }

    // Classes
    if (!m_classes.empty()) {
        api << "## Classes\n\n";
        for (const auto& cls : m_classes) {
            api << "### " << cls.name << "\n\n";
            if (!cls.namespace_.empty()) {
                api << "**Namespace:** `" << cls.namespace_ << "`\n\n";
            }
            api << cls.description << "\n\n";

            if (!cls.methods.empty()) {
                api << "#### Methods\n\n";
                for (const auto& method : cls.methods) {
                    api << "- `" << method << "()` - Method " << method << "\n";
                }
                api << "\n";
            }
        }
    }

    // Functions
    if (!m_functions.empty()) {
        api << "## Functions\n\n";
        for (const auto& func : m_functions) {
            api << "### " << func.name << "\n\n";
            if (!func.namespace_.empty()) {
                api << "**Namespace:** `" << func.namespace_ << "`\n\n";
            }
            api << "**Return Type:** `" << func.returnType << "`\n\n";
            api << func.description << "\n\n";
        }
    }

    if (m_classes.empty() && m_functions.empty()) {
        api << "No API documentation found. Make sure your source files contain properly formatted classes and functions.\n\n";
    }

    return api.str();
}

// APIDocExtractor getter implementations
std::vector<NamespaceInfo> APIDocExtractor::getNamespaces() const {
    return m_namespaces;
}

std::vector<ClassInfo> APIDocExtractor::getClasses() const {
    return m_classes;
}

std::vector<FunctionInfo> APIDocExtractor::getFunctions() const {
    return m_functions;
}

// Enhanced API documentation generation methods
std::string DocGenerator::generateAPIIndexContent(const APIDocExtractor& extractor) {
    std::ostringstream index;

    index << "# {{PROJECT_NAME}} API Reference\n\n";
    index << "Welcome to the {{PROJECT_NAME}} API documentation. This comprehensive reference covers all public APIs, classes, and functions.\n\n";

    // Table of Contents
    index << "## Table of Contents\n\n";

    auto namespaces = extractor.getNamespaces();
    auto classes = extractor.getClasses();
    auto functions = extractor.getFunctions();

    if (!namespaces.empty()) {
        index << "### Namespaces\n\n";
        for (const auto& ns : namespaces) {
            index << "- [" << ns.name << "](" << ns.name << ".md) - " << ns.description << "\n";
        }
        index << "\n";
    }

    if (!classes.empty()) {
        index << "### Classes\n\n";
        for (const auto& cls : classes) {
            index << "- [" << cls.name << "](classes/" << cls.name << ".md) - " << cls.description << "\n";
        }
        index << "\n";
    }

    if (!functions.empty()) {
        index << "### Global Functions\n\n";
        for (const auto& func : functions) {
            index << "- `" << func.returnType << " " << func.name << "()` - " << func.description << "\n";
        }
        index << "\n";
    }

    // Quick Start
    index << "## Quick Start\n\n";
    index << "```cpp\n";
    index << "#include <{{PROJECT_NAME}}/{{PROJECT_NAME}}.h>\n\n";
    index << "int main() {\n";
    index << "    // Basic usage example\n";
    index << "    {{PROJECT_NAME}}::initialize();\n";
    index << "    \n";
    index << "    // Your code here\n";
    index << "    \n";
    index << "    {{PROJECT_NAME}}::cleanup();\n";
    index << "    return 0;\n";
    index << "}\n";
    index << "```\n\n";

    // Navigation
    index << "## Navigation\n\n";
    index << "- [Complete API Reference](reference.md) - Full API documentation\n";
    index << "- [User Guide](../user-guide.md) - Usage examples and tutorials\n";
    index << "- [Developer Guide](../developer-guide.md) - Contributing and development\n\n";

    index << "---\n\n";
    index << "*Generated automatically by CPP-Scaffold*\n";

    return index.str();
}

std::string DocGenerator::generateNamespaceDocumentation(const NamespaceInfo& ns) {
    std::ostringstream doc;

    doc << "# Namespace: " << ns.name << "\n\n";
    doc << ns.description << "\n\n";

    // Classes in this namespace
    if (!ns.classes.empty()) {
        doc << "## Classes\n\n";
        for (const auto& cls : ns.classes) {
            doc << "### " << cls.name << "\n\n";
            doc << cls.description << "\n\n";

            if (!cls.methods.empty()) {
                doc << "**Methods:**\n";
                for (const auto& method : cls.methods) {
                    doc << "- `" << method << "`\n";
                }
                doc << "\n";
            }

            if (!cls.members.empty()) {
                doc << "**Members:**\n";
                for (const auto& member : cls.members) {
                    doc << "- `" << member << "`\n";
                }
                doc << "\n";
            }
        }
    }

    // Functions in this namespace
    if (!ns.functions.empty()) {
        doc << "## Functions\n\n";
        for (const auto& func : ns.functions) {
            doc << "### " << func.name << "\n\n";
            doc << "```cpp\n";
            doc << func.returnType << " " << func.name << "(";

            for (size_t i = 0; i < func.parameters.size(); ++i) {
                if (i > 0) doc << ", ";
                doc << func.parameters[i].first << " " << func.parameters[i].second;
            }

            doc << ")";
            if (func.isConst) doc << " const";
            doc << ";\n```\n\n";

            doc << func.description << "\n\n";

            if (!func.parameters.empty()) {
                doc << "**Parameters:**\n";
                for (const auto& param : func.parameters) {
                    doc << "- `" << param.second << "` (" << param.first << ")\n";
                }
                doc << "\n";
            }
        }
    }

    // Enums in this namespace
    if (!ns.enums.empty()) {
        doc << "## Enumerations\n\n";
        for (const auto& enumName : ns.enums) {
            doc << "- `" << enumName << "`\n";
        }
        doc << "\n";
    }

    // Typedefs in this namespace
    if (!ns.typedefs.empty()) {
        doc << "## Type Definitions\n\n";
        for (const auto& typedef_ : ns.typedefs) {
            doc << "- `" << typedef_ << "`\n";
        }
        doc << "\n";
    }

    return doc.str();
}

std::string DocGenerator::generateClassDocumentation(const ClassInfo& cls) {
    std::ostringstream doc;

    doc << "# Class: " << cls.name << "\n\n";

    // Namespace information
    if (!cls.namespace_.empty()) {
        doc << "**Namespace:** `" << cls.namespace_ << "`\n\n";
    }

    // Base classes
    if (!cls.baseClasses.empty()) {
        doc << "**Inherits from:** ";
        for (size_t i = 0; i < cls.baseClasses.size(); ++i) {
            if (i > 0) doc << ", ";
            doc << "`" << cls.baseClasses[i] << "`";
        }
        doc << "\n\n";
    }

    doc << cls.description << "\n\n";

    // Class declaration
    doc << "## Declaration\n\n";
    doc << "```cpp\n";
    doc << "class " << cls.name;
    if (!cls.baseClasses.empty()) {
        doc << " : ";
        for (size_t i = 0; i < cls.baseClasses.size(); ++i) {
            if (i > 0) doc << ", ";
            doc << "public " << cls.baseClasses[i];
        }
    }
    doc << " {\n";
    doc << "    // Implementation details...\n";
    doc << "};\n";
    doc << "```\n\n";

    // Methods
    if (!cls.methods.empty()) {
        doc << "## Methods\n\n";
        for (const auto& method : cls.methods) {
            doc << "### " << method << "\n\n";

            auto methodDocIt = cls.methodDocs.find(method);
            if (methodDocIt != cls.methodDocs.end()) {
                doc << methodDocIt->second << "\n\n";
            }

            doc << "```cpp\n";
            doc << method << ";\n";
            doc << "```\n\n";
        }
    }

    // Members
    if (!cls.members.empty()) {
        doc << "## Members\n\n";
        for (const auto& member : cls.members) {
            doc << "### " << member << "\n\n";

            auto memberDocIt = cls.memberDocs.find(member);
            if (memberDocIt != cls.memberDocs.end()) {
                doc << memberDocIt->second << "\n\n";
            }

            doc << "```cpp\n";
            doc << member << ";\n";
            doc << "```\n\n";
        }
    }

    // Usage example
    doc << "## Usage Example\n\n";
    doc << "```cpp\n";
    doc << "#include <" << cls.namespace_ << "/" << cls.name << ".h>\n\n";
    doc << "int main() {\n";
    doc << "    " << cls.namespace_ << "::" << cls.name << " obj;\n";
    doc << "    \n";
    doc << "    // Use the object\n";
    doc << "    // obj.someMethod();\n";
    doc << "    \n";
    doc << "    return 0;\n";
    doc << "}\n";
    doc << "```\n\n";

    return doc.str();
}

} // namespace documentation
