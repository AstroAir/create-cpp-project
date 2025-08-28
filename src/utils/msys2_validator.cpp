#include "msys2_validator.h"
#include "file_utils.h"
#include "terminal_utils.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <map>
#include <cstdlib>
#include <algorithm>

MSYS2Validator::ValidationResult MSYS2Validator::validatePKGBUILD(const std::string& pkgbuildPath) {
    ValidationResult result;
    result.isValid = false;

    // Check if file exists
    if (!FileUtils::fileExists(pkgbuildPath)) {
        result.errors.push_back("PKGBUILD file not found: " + pkgbuildPath);
        return result;
    }

    // Read file content
    std::string content = FileUtils::readFile(pkgbuildPath);
    if (content.empty()) {
        result.errors.push_back("Failed to read PKGBUILD file or file is empty");
        return result;
    }

    return validatePKGBUILDContent(content);
}

MSYS2Validator::ValidationResult MSYS2Validator::validatePKGBUILDContent(const std::string& pkgbuildContent) {
    ValidationResult result;
    result.isValid = true;

    // Validate syntax
    auto syntaxResult = validatePKGBUILDSyntax(pkgbuildContent);
    result.errors.insert(result.errors.end(), syntaxResult.errors.begin(), syntaxResult.errors.end());
    result.warnings.insert(result.warnings.end(), syntaxResult.warnings.begin(), syntaxResult.warnings.end());
    
    if (!syntaxResult.isValid) {
        result.isValid = false;
    }

    // Parse PKGBUILD variables
    auto pkgbuildVars = parsePKGBUILD(pkgbuildContent);

    // Validate required fields
    auto fieldsResult = validateRequiredFields(pkgbuildVars);
    result.errors.insert(result.errors.end(), fieldsResult.errors.begin(), fieldsResult.errors.end());
    result.warnings.insert(result.warnings.end(), fieldsResult.warnings.begin(), fieldsResult.warnings.end());
    
    if (!fieldsResult.isValid) {
        result.isValid = false;
    }

    // Validate array fields
    auto arrayResult = validateArrayFields(pkgbuildVars);
    result.errors.insert(result.errors.end(), arrayResult.errors.begin(), arrayResult.errors.end());
    result.warnings.insert(result.warnings.end(), arrayResult.warnings.begin(), arrayResult.warnings.end());

    // Add suggestions
    if (pkgbuildVars.find("pkgdesc") != pkgbuildVars.end()) {
        if (pkgbuildVars["pkgdesc"].length() > 80) {
            result.suggestions.push_back("Consider shortening package description (currently > 80 characters)");
        }
    }

    return result;
}

MSYS2Validator::ValidationResult MSYS2Validator::validateMSYS2Environment() {
    ValidationResult result;
    result.isValid = true;

    // Check if we're in MSYS2 environment
    if (!isMSYS2Environment()) {
        result.errors.push_back("Not running in MSYS2 environment");
        result.suggestions.push_back("Please run this command in MSYS2 MinGW64 or MinGW32 shell");
        result.isValid = false;
        return result;
    }

    // Check for pacman
    std::string pacmanVersion = executeCommand("pacman --version");
    if (pacmanVersion.empty()) {
        result.errors.push_back("pacman package manager not found");
        result.isValid = false;
    } else {
        result.suggestions.push_back("pacman found: " + pacmanVersion.substr(0, pacmanVersion.find('\n')));
    }

    // Check MINGW_PREFIX
    const char* mingwPrefix = std::getenv("MINGW_PREFIX");
    if (!mingwPrefix) {
        result.warnings.push_back("MINGW_PREFIX environment variable not set");
    } else {
        result.suggestions.push_back("MINGW_PREFIX: " + std::string(mingwPrefix));
    }

    // Validate build tools
    auto buildToolsResult = validateBuildTools();
    result.errors.insert(result.errors.end(), buildToolsResult.errors.begin(), buildToolsResult.errors.end());
    result.warnings.insert(result.warnings.end(), buildToolsResult.warnings.begin(), buildToolsResult.warnings.end());
    
    if (!buildToolsResult.isValid) {
        result.isValid = false;
    }

    return result;
}

MSYS2Validator::BuildTestResult MSYS2Validator::testMSYS2Build(const std::string& projectPath) {
    BuildTestResult result;
    result.buildSuccessful = false;
    result.testsSuccessful = false;

    // Check if PKGBUILD exists
    std::string pkgbuildPath = FileUtils::combinePath(projectPath, "PKGBUILD");
    if (!FileUtils::fileExists(pkgbuildPath)) {
        result.errors.push_back("PKGBUILD not found in project directory");
        return result;
    }

    // Validate PKGBUILD first
    auto validationResult = validatePKGBUILD(pkgbuildPath);
    if (!validationResult.isValid) {
        result.errors.push_back("PKGBUILD validation failed");
        for (const auto& error : validationResult.errors) {
            result.errors.push_back("  " + error);
        }
        return result;
    }

    // TODO: Implement actual build testing
    // This would involve:
    // 1. Creating a temporary build directory
    // 2. Running makepkg -s to build the package
    // 3. Checking build output for errors
    // 4. Running tests if available
    
    result.buildOutput = "Build testing not yet implemented";
    result.testOutput = "Test execution not yet implemented";
    result.suggestions.push_back("Manual testing: cd to project directory and run 'makepkg -s'");

    return result;
}

std::string MSYS2Validator::executeCommand(const std::string& command) {
    std::string result;
    char buffer[128];
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    pclose(pipe);
    return result;
}

bool MSYS2Validator::isMSYS2Environment() {
    // Check for MSYS2-specific environment variables
    const char* msystem = std::getenv("MSYSTEM");
    const char* mingwPrefix = std::getenv("MINGW_PREFIX");
    
    return (msystem != nullptr) || (mingwPrefix != nullptr);
}

std::map<std::string, std::string> MSYS2Validator::parsePKGBUILD(const std::string& content) {
    std::map<std::string, std::string> vars;
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Look for variable assignments
        std::regex varRegex(R"(^\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*(.*)$)");
        std::smatch match;
        
        if (std::regex_match(line, match, varRegex)) {
            std::string varName = match[1].str();
            std::string varValue = match[2].str();
            
            // Remove quotes if present
            if (varValue.length() >= 2 && 
                ((varValue.front() == '"' && varValue.back() == '"') ||
                 (varValue.front() == '\'' && varValue.back() == '\''))) {
                varValue = varValue.substr(1, varValue.length() - 2);
            }
            
            vars[varName] = varValue;
        }
    }
    
    return vars;
}

MSYS2Validator::ValidationResult MSYS2Validator::validateRequiredFields(const std::map<std::string, std::string>& pkgbuildVars) {
    ValidationResult result;
    result.isValid = true;
    
    // Required fields for MSYS2 packages
    std::vector<std::string> requiredFields = {
        "pkgname", "pkgver", "pkgrel", "pkgdesc", "arch", "license"
    };
    
    for (const auto& field : requiredFields) {
        if (pkgbuildVars.find(field) == pkgbuildVars.end() || pkgbuildVars.at(field).empty()) {
            result.errors.push_back("Missing required field: " + field);
            result.isValid = false;
        }
    }
    
    // Check for MSYS2-specific fields
    if (pkgbuildVars.find("mingw_arch") == pkgbuildVars.end()) {
        result.warnings.push_back("Missing mingw_arch field (recommended for MSYS2 packages)");
    }
    
    return result;
}

MSYS2Validator::ValidationResult MSYS2Validator::validateArrayFields(const std::map<std::string, std::string>& pkgbuildVars) {
    ValidationResult result;
    result.isValid = true;
    
    // Check common array fields
    std::vector<std::string> arrayFields = {"depends", "makedepends", "source"};
    
    for (const auto& field : arrayFields) {
        auto it = pkgbuildVars.find(field);
        if (it != pkgbuildVars.end()) {
            const std::string& value = it->second;
            // Basic validation - should start with ( and end with )
            if (!value.empty() && !(value.front() == '(' && value.back() == ')')) {
                result.warnings.push_back("Array field " + field + " should be enclosed in parentheses");
            }
        }
    }
    
    return result;
}

MSYS2Validator::ValidationResult MSYS2Validator::validatePKGBUILDSyntax(const std::string& content) {
    ValidationResult result;
    result.isValid = true;
    
    // Basic syntax checks
    if (content.empty()) {
        result.errors.push_back("PKGBUILD content is empty");
        result.isValid = false;
        return result;
    }
    
    // Check for required functions
    if (content.find("build()") == std::string::npos) {
        result.warnings.push_back("No build() function found");
    }
    
    if (content.find("package()") == std::string::npos) {
        result.errors.push_back("Missing required package() function");
        result.isValid = false;
    }
    
    return result;
}

MSYS2Validator::ValidationResult MSYS2Validator::validateBuildTools() {
    ValidationResult result;
    result.isValid = true;
    
    // Check for essential build tools
    std::vector<std::pair<std::string, std::string>> tools = {
        {"cmake", "cmake --version"},
        {"ninja", "ninja --version"},
        {"gcc", "gcc --version"},
        {"make", "make --version"}
    };
    
    for (const auto& tool : tools) {
        std::string output = executeCommand(tool.second);
        if (output.empty()) {
            result.warnings.push_back("Build tool not found: " + tool.first);
        }
    }

    return result;
}

MSYS2Validator::ValidationResult MSYS2Validator::validateDependencies(const std::vector<std::string>& dependencies) {
    ValidationResult result;
    result.isValid = true;

    if (!isMSYS2Environment()) {
        result.errors.push_back("Not in MSYS2 environment - cannot validate dependencies");
        result.isValid = false;
        return result;
    }

    for (const auto& dep : dependencies) {
        if (!isPackageInstalled(dep)) {
            result.warnings.push_back("Dependency not installed: " + dep);
            result.suggestions.push_back("Install with: pacman -S " + dep);
        }
    }

    return result;
}

std::optional<std::string> MSYS2Validator::getPackageInfo(const std::string& packageName) {
    if (!isMSYS2Environment()) {
        return std::nullopt;
    }

    std::string command = "pacman -Si " + packageName + " 2>/dev/null";
    std::string output = executeCommand(command);

    if (output.empty()) {
        return std::nullopt;
    }

    return output;
}

bool MSYS2Validator::isPackageInstalled(const std::string& packageName) {
    if (!isMSYS2Environment()) {
        return false;
    }

    std::string command = "pacman -Q " + packageName + " 2>/dev/null";
    std::string output = executeCommand(command);

    return !output.empty();
}
