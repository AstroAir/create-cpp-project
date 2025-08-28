#pragma once

#include <string>
#include <vector>
#include <optional>
#include <map>

/**
 * @brief MSYS2 package validation and testing utilities
 * 
 * This class provides functionality to validate PKGBUILD files,
 * test MSYS2 build processes, and ensure MSYS2 environment compatibility.
 */
class MSYS2Validator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
    };

    struct BuildTestResult {
        bool buildSuccessful;
        bool testsSuccessful;
        std::string buildOutput;
        std::string testOutput;
        std::vector<std::string> errors;
    };

    /**
     * @brief Validate a PKGBUILD file for correctness
     * @param pkgbuildPath Path to the PKGBUILD file
     * @return ValidationResult with validation status and messages
     */
    static ValidationResult validatePKGBUILD(const std::string& pkgbuildPath);

    /**
     * @brief Validate PKGBUILD content directly
     * @param pkgbuildContent Content of the PKGBUILD file
     * @return ValidationResult with validation status and messages
     */
    static ValidationResult validatePKGBUILDContent(const std::string& pkgbuildContent);

    /**
     * @brief Check if MSYS2 environment is properly configured
     * @return ValidationResult with environment status
     */
    static ValidationResult validateMSYS2Environment();

    /**
     * @brief Test building a package in MSYS2 environment
     * @param projectPath Path to the project directory
     * @return BuildTestResult with build and test results
     */
    static BuildTestResult testMSYS2Build(const std::string& projectPath);

    /**
     * @brief Validate MSYS2 dependencies are available
     * @param dependencies List of MSYS2 package dependencies
     * @return ValidationResult with dependency status
     */
    static ValidationResult validateDependencies(const std::vector<std::string>& dependencies);

    /**
     * @brief Get MSYS2 package information
     * @param packageName Name of the MSYS2 package
     * @return Optional package information string
     */
    static std::optional<std::string> getPackageInfo(const std::string& packageName);

    /**
     * @brief Check if a specific MSYS2 package is installed
     * @param packageName Name of the package to check
     * @return True if package is installed, false otherwise
     */
    static bool isPackageInstalled(const std::string& packageName);

    /**
     * @brief Validate PKGBUILD syntax and required fields
     * @param content PKGBUILD file content
     * @return ValidationResult with syntax validation results
     */
    static ValidationResult validatePKGBUILDSyntax(const std::string& content);

    /**
     * @brief Check MSYS2 build tools availability
     * @return ValidationResult with build tools status
     */
    static ValidationResult validateBuildTools();

private:
    /**
     * @brief Execute a command and capture output
     * @param command Command to execute
     * @return Command output or empty string on failure
     */
    static std::string executeCommand(const std::string& command);

    /**
     * @brief Check if we're running in MSYS2 environment
     * @return True if in MSYS2 environment
     */
    static bool isMSYS2Environment();

    /**
     * @brief Parse PKGBUILD file and extract key information
     * @param content PKGBUILD file content
     * @return Map of PKGBUILD variables and their values
     */
    static std::map<std::string, std::string> parsePKGBUILD(const std::string& content);

    /**
     * @brief Validate required PKGBUILD fields
     * @param pkgbuildVars Parsed PKGBUILD variables
     * @return ValidationResult for required fields
     */
    static ValidationResult validateRequiredFields(const std::map<std::string, std::string>& pkgbuildVars);

    /**
     * @brief Validate PKGBUILD array fields (depends, makedepends, etc.)
     * @param pkgbuildVars Parsed PKGBUILD variables
     * @return ValidationResult for array fields
     */
    static ValidationResult validateArrayFields(const std::map<std::string, std::string>& pkgbuildVars);
};
