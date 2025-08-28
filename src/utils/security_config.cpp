#include "security_config.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <unordered_map>

namespace utils {

SecurityConfigManager::SecurityConfigManager() {
    initializeDefaultConfigs();

    // Initialize analyzer commands
    analyzerCommands_[StaticAnalyzer::ClangStaticAnalyzer] = "clang-static-analyzer";
    analyzerCommands_[StaticAnalyzer::CppCheck] = "cppcheck";
    analyzerCommands_[StaticAnalyzer::PVSStudio] = "pvs-studio-analyzer";
    analyzerCommands_[StaticAnalyzer::SonarQube] = "sonar-scanner";
    analyzerCommands_[StaticAnalyzer::Clazy] = "clazy";
    analyzerCommands_[StaticAnalyzer::IncludeWhatYouUse] = "include-what-you-use";

    // Initialize sanitizer flags
    sanitizerFlags_[Sanitizer::AddressSanitizer] = "-fsanitize=address";
    sanitizerFlags_[Sanitizer::ThreadSanitizer] = "-fsanitize=thread";
    sanitizerFlags_[Sanitizer::UndefinedBehaviorSanitizer] = "-fsanitize=undefined";
    sanitizerFlags_[Sanitizer::MemorySanitizer] = "-fsanitize=memory";
    sanitizerFlags_[Sanitizer::LeakSanitizer] = "-fsanitize=leak";
    sanitizerFlags_[Sanitizer::ControlFlowIntegrity] = "-fsanitize=cfi";
}

SecurityConfig SecurityConfigManager::getDefaultConfig(SecurityLevel level) const {
    auto it = defaultConfigs_.find(level);
    if (it != defaultConfigs_.end()) {
        return it->second;
    }
    return defaultConfigs_.at(SecurityLevel::Basic);
}

SecurityConfig SecurityConfigManager::createCustomConfig(const std::vector<std::string>& options) const {
    SecurityConfig config = getDefaultConfig(SecurityLevel::Basic);

    for (const auto& option : options) {
        if (option == "enable-asan") {
            config.sanitizers.push_back(Sanitizer::AddressSanitizer);
        } else if (option == "enable-tsan") {
            config.sanitizers.push_back(Sanitizer::ThreadSanitizer);
        } else if (option == "enable-ubsan") {
            config.sanitizers.push_back(Sanitizer::UndefinedBehaviorSanitizer);
        } else if (option == "enable-cppcheck") {
            config.staticAnalyzers.push_back(StaticAnalyzer::CppCheck);
        } else if (option == "enable-clang-analyzer") {
            config.staticAnalyzers.push_back(StaticAnalyzer::ClangStaticAnalyzer);
        } else if (option == "paranoid") {
            config = getDefaultConfig(SecurityLevel::Paranoid);
        } else if (option == "enhanced") {
            config = getDefaultConfig(SecurityLevel::Enhanced);
        }
    }

    return config;
}

bool SecurityConfigManager::validateConfig(const SecurityConfig& config) const {
    // Check for conflicting sanitizers
    bool hasASan = std::find(config.sanitizers.begin(), config.sanitizers.end(),
                            Sanitizer::AddressSanitizer) != config.sanitizers.end();
    bool hasTSan = std::find(config.sanitizers.begin(), config.sanitizers.end(),
                            Sanitizer::ThreadSanitizer) != config.sanitizers.end();
    bool hasMSan = std::find(config.sanitizers.begin(), config.sanitizers.end(),
                            Sanitizer::MemorySanitizer) != config.sanitizers.end();

    if ((hasASan && hasTSan) || (hasASan && hasMSan) || (hasTSan && hasMSan)) {
        spdlog::warn("Conflicting sanitizers detected. ASan, TSan, and MSan cannot be used together.");
        return false;
    }

    return true;
}

std::string SecurityConfigManager::generateCMakeSecurityConfig(const SecurityConfig& config) const {
    std::string cmake_config;

    // Security flags
    if (!config.securityFlags.empty()) {
        cmake_config += "# Security hardening flags\n";
        cmake_config += "set(SECURITY_FLAGS\n";
        for (const auto& flag : config.securityFlags) {
            std::string flag_str = getSecurityFlagString(flag, "gcc");
            if (!flag_str.empty()) {
                cmake_config += fmt::format("    {}\n", flag_str);
            }
        }
        cmake_config += ")\n\n";

        cmake_config += "target_compile_options(${PROJECT_NAME} PRIVATE ${SECURITY_FLAGS})\n";
        cmake_config += "target_link_options(${PROJECT_NAME} PRIVATE ${SECURITY_FLAGS})\n\n";
    }

    // Sanitizers
    if (!config.sanitizers.empty()) {
        cmake_config += "# Sanitizer configuration\n";
        cmake_config += "if(CMAKE_BUILD_TYPE STREQUAL \"Debug\")\n";
        cmake_config += "    set(SANITIZER_FLAGS\n";
        for (const auto& sanitizer : config.sanitizers) {
            cmake_config += fmt::format("        {}\n", getSanitizerFlag(sanitizer));
        }
        cmake_config += "    )\n";
        cmake_config += "    target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS})\n";
        cmake_config += "    target_link_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS})\n";
        cmake_config += "endif()\n\n";
    }

    // Static analysis
    if (!config.staticAnalyzers.empty()) {
        cmake_config += "# Static analysis configuration\n";
        for (const auto& analyzer : config.staticAnalyzers) {
            if (analyzer == StaticAnalyzer::ClangStaticAnalyzer) {
                cmake_config += "set(CMAKE_CXX_CLANG_TIDY clang-tidy)\n";
            } else if (analyzer == StaticAnalyzer::CppCheck) {
                cmake_config += "find_program(CPPCHECK_EXECUTABLE cppcheck)\n";
                cmake_config += "if(CPPCHECK_EXECUTABLE)\n";
                cmake_config += "    set(CMAKE_CXX_CPPCHECK ${CPPCHECK_EXECUTABLE} --enable=all --std=c++17)\n";
                cmake_config += "endif()\n";
            } else if (analyzer == StaticAnalyzer::IncludeWhatYouUse) {
                cmake_config += "find_program(IWYU_EXECUTABLE include-what-you-use)\n";
                cmake_config += "if(IWYU_EXECUTABLE)\n";
                cmake_config += "    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXECUTABLE})\n";
                cmake_config += "endif()\n";
            }
        }
        cmake_config += "\n";
    }

    // Warning configuration
    if (config.enableExtraWarnings) {
        cmake_config += "# Enhanced warning configuration\n";
        cmake_config += "target_compile_options(${PROJECT_NAME} PRIVATE\n";
        cmake_config += "    $<$<COMPILE_LANGUAGE:CXX>:\n";
        cmake_config += "        -Wall -Wextra -Wpedantic\n";
        cmake_config += "        -Wconversion -Wsign-conversion\n";
        cmake_config += "        -Wcast-qual -Wcast-align\n";
        cmake_config += "        -Wshadow -Wnon-virtual-dtor\n";
        cmake_config += "        -Wold-style-cast -Woverloaded-virtual\n";
        cmake_config += "    >\n";
        cmake_config += ")\n\n";
    }

    if (config.enableWarningsAsErrors) {
        cmake_config += "# Treat warnings as errors\n";
        cmake_config += "target_compile_options(${PROJECT_NAME} PRIVATE -Werror)\n\n";
    }

    return cmake_config;
}

std::string SecurityConfigManager::generateSanitizerFlags(const SecurityConfig& config, bool isDebug) const {
    if (!isDebug && !config.enableSanitizersInRelease) {
        return "";
    }

    std::string flags;
    for (const auto& sanitizer : config.sanitizers) {
        if (!flags.empty()) flags += " ";
        flags += getSanitizerFlag(sanitizer);
    }

    return flags;
}

std::string SecurityConfigManager::generateSecurityFlags(const SecurityConfig& config, const std::string& compiler) const {
    std::string flags;

    for (const auto& flag : config.securityFlags) {
        std::string flag_str = getSecurityFlagString(flag, compiler);
        if (!flag_str.empty()) {
            if (!flags.empty()) flags += " ";
            flags += flag_str;
        }
    }

    return flags;
}

void SecurityConfigManager::initializeDefaultConfigs() {
    // Basic security configuration
    SecurityConfig basic;
    basic.level = SecurityLevel::Basic;
    basic.staticAnalyzers = {StaticAnalyzer::CppCheck};
    basic.sanitizers = {Sanitizer::AddressSanitizer};
    basic.securityFlags = {SecurityFlag::StackProtector, SecurityFlag::Fortify};
    basic.enableDependencyScanning = true;
    basic.enableExtraWarnings = true;
    defaultConfigs_[SecurityLevel::Basic] = basic;

    // Enhanced security configuration
    SecurityConfig enhanced;
    enhanced.level = SecurityLevel::Enhanced;
    enhanced.staticAnalyzers = {StaticAnalyzer::ClangStaticAnalyzer, StaticAnalyzer::CppCheck};
    enhanced.sanitizers = {Sanitizer::AddressSanitizer, Sanitizer::UndefinedBehaviorSanitizer};
    enhanced.securityFlags = {SecurityFlag::StackProtector, SecurityFlag::Fortify,
                             SecurityFlag::RelRO, SecurityFlag::NowBinding};
    enhanced.enableDependencyScanning = true;
    enhanced.enableVulnerabilityChecks = true;
    enhanced.enableExtraWarnings = true;
    enhanced.scanForSecrets = true;
    defaultConfigs_[SecurityLevel::Enhanced] = enhanced;

    // Paranoid security configuration
    SecurityConfig paranoid;
    paranoid.level = SecurityLevel::Paranoid;
    paranoid.staticAnalyzers = {StaticAnalyzer::ClangStaticAnalyzer, StaticAnalyzer::CppCheck,
                               StaticAnalyzer::PVSStudio, StaticAnalyzer::IncludeWhatYouUse};
    paranoid.sanitizers = {Sanitizer::AddressSanitizer, Sanitizer::UndefinedBehaviorSanitizer,
                          Sanitizer::ControlFlowIntegrity};
    paranoid.securityFlags = {SecurityFlag::StackProtector, SecurityFlag::Fortify,
                             SecurityFlag::RelRO, SecurityFlag::NowBinding,
                             SecurityFlag::NoExecutableStack, SecurityFlag::PieExecutable};
    paranoid.enableDependencyScanning = true;
    paranoid.enableVulnerabilityChecks = true;
    paranoid.enableWarningsAsErrors = true;
    paranoid.enableExtraWarnings = true;
    paranoid.scanForSecrets = true;
    paranoid.enableFuzzTesting = true;
    paranoid.failBuildOnStaticAnalysisErrors = true;
    defaultConfigs_[SecurityLevel::Paranoid] = paranoid;

    // None configuration (minimal security)
    SecurityConfig none;
    none.level = SecurityLevel::None;
    defaultConfigs_[SecurityLevel::None] = none;
}

std::string SecurityConfigManager::getSecurityFlagString(SecurityFlag flag, const std::string& compiler) const {
    switch (flag) {
        case SecurityFlag::StackProtector:
            return "-fstack-protector-strong";
        case SecurityFlag::Fortify:
            return "-D_FORTIFY_SOURCE=2";
        case SecurityFlag::RelRO:
            return "-Wl,-z,relro";
        case SecurityFlag::NowBinding:
            return "-Wl,-z,now";
        case SecurityFlag::NoExecutableStack:
            return "-Wl,-z,noexecstack";
        case SecurityFlag::PieExecutable:
            return "-fPIE";
        case SecurityFlag::ShadowStack:
            return "-fcf-protection=full";
        case SecurityFlag::ControlFlowGuard:
            return compiler == "msvc" ? "/guard:cf" : "";
        default:
            return "";
    }
}

std::string SecurityConfigManager::getSanitizerFlag(Sanitizer sanitizer) const {
    auto it = sanitizerFlags_.find(sanitizer);
    return it != sanitizerFlags_.end() ? it->second : "";
}

std::string SecurityConfigManager::generateMesonSecurityConfig(const SecurityConfig& config) const {
    std::string meson_config;

    // Security flags
    if (!config.securityFlags.empty()) {
        meson_config += "# Security hardening flags\n";
        meson_config += "security_flags = [\n";
        for (const auto& flag : config.securityFlags) {
            std::string flag_str = getSecurityFlagString(flag, "gcc");
            if (!flag_str.empty()) {
                meson_config += fmt::format("  '{}',\n", flag_str);
            }
        }
        meson_config += "]\n\n";
        meson_config += "add_project_arguments(security_flags, language: 'cpp')\n";
        meson_config += "add_project_link_arguments(security_flags, language: 'cpp')\n\n";
    }

    // Sanitizers for debug builds
    if (!config.sanitizers.empty()) {
        meson_config += "# Sanitizer configuration\n";
        meson_config += "if get_option('buildtype') == 'debug'\n";
        meson_config += "  sanitizer_flags = [\n";
        for (const auto& sanitizer : config.sanitizers) {
            meson_config += fmt::format("    '{}',\n", getSanitizerFlag(sanitizer));
        }
        meson_config += "  ]\n";
        meson_config += "  add_project_arguments(sanitizer_flags, language: 'cpp')\n";
        meson_config += "  add_project_link_arguments(sanitizer_flags, language: 'cpp')\n";
        meson_config += "endif\n\n";
    }

    return meson_config;
}

std::string SecurityConfigManager::generateBazelSecurityConfig(const SecurityConfig& config) const {
    std::string bazel_config;

    // Security flags
    if (!config.securityFlags.empty()) {
        bazel_config += "# Security hardening flags\n";
        bazel_config += "SECURITY_COPTS = [\n";
        for (const auto& flag : config.securityFlags) {
            std::string flag_str = getSecurityFlagString(flag, "gcc");
            if (!flag_str.empty()) {
                bazel_config += fmt::format("    \"{}\",\n", flag_str);
            }
        }
        bazel_config += "]\n\n";

        bazel_config += "SECURITY_LINKOPTS = [\n";
        for (const auto& flag : config.securityFlags) {
            std::string flag_str = getSecurityFlagString(flag, "gcc");
            if (flag_str.find("-Wl,") == 0) {
                bazel_config += fmt::format("    \"{}\",\n", flag_str);
            }
        }
        bazel_config += "]\n\n";
    }

    return bazel_config;
}

std::string SecurityConfigManager::generateClangStaticAnalyzerConfig(const SecurityConfig& config) const {
    if (std::find(config.staticAnalyzers.begin(), config.staticAnalyzers.end(),
                  StaticAnalyzer::ClangStaticAnalyzer) == config.staticAnalyzers.end()) {
        return "";
    }

    return R"(# Clang Static Analyzer Configuration
Checks: >
  -*,
  bugprone-*,
  cert-*,
  clang-analyzer-*,
  cppcoreguidelines-*,
  hicpp-*,
  misc-*,
  modernize-*,
  performance-*,
  portability-*,
  readability-*,
  security-*

CheckOptions:
  - key: cert-err58-cpp.CheckThrowTemporaries
    value: true
  - key: cert-oop57-cpp.MemSetNames
    value: 'memset;bzero'
  - key: modernize-use-nullptr.NullMacros
    value: 'NULL'
)";
}

std::string SecurityConfigManager::generateCppCheckConfig(const SecurityConfig& config) const {
    if (std::find(config.staticAnalyzers.begin(), config.staticAnalyzers.end(),
                  StaticAnalyzer::CppCheck) == config.staticAnalyzers.end()) {
        return "";
    }

    return R"(<?xml version="1.0" encoding="UTF-8"?>
<project version="1">
    <root name="."/>
    <builddir>build</builddir>
    <analyze-all-vs-configs>true</analyze-all-vs-configs>
    <check-headers>true</check-headers>
    <check-unused-templates>true</check-unused-templates>
    <max-ctu-depth>2</max-ctu-depth>
    <max-template-recursion>100</max-template-recursion>
    <suppressions>
        <suppression>missingIncludeSystem</suppression>
    </suppressions>
    <libraries>
        <library>std</library>
        <library>posix</library>
    </libraries>
    <platform>native</platform>
</project>
)";
}

std::string SecurityConfigManager::generatePVSStudioConfig(const SecurityConfig& config) const {
    if (std::find(config.staticAnalyzers.begin(), config.staticAnalyzers.end(),
                  StaticAnalyzer::PVSStudio) == config.staticAnalyzers.end()) {
        return "";
    }

    return R"(# PVS-Studio Configuration
exclude-path = build/
exclude-path = tests/
platform = linux64
preprocessor = gcc
language = C++
skip-cl-exe = yes
)";
}

std::string SecurityConfigManager::generateSanitizerCMakeConfig(const SecurityConfig& config) const {
    if (config.sanitizers.empty()) {
        return "";
    }

    std::string cmake_config = "# Sanitizer CMake Configuration\n";
    cmake_config += "option(ENABLE_SANITIZERS \"Enable sanitizers\" OFF)\n\n";

    cmake_config += "if(ENABLE_SANITIZERS)\n";
    cmake_config += "    set(SANITIZER_FLAGS\n";
    for (const auto& sanitizer : config.sanitizers) {
        cmake_config += fmt::format("        {}\n", getSanitizerFlag(sanitizer));
    }
    cmake_config += "    )\n";
    cmake_config += "    \n";
    cmake_config += "    target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS})\n";
    cmake_config += "    target_link_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS})\n";
    cmake_config += "    \n";
    cmake_config += "    # Set environment variables for sanitizers\n";
    cmake_config += "    set_property(TEST ${PROJECT_NAME}_tests PROPERTY\n";
    cmake_config += "        ENVIRONMENT \"ASAN_OPTIONS=detect_leaks=1:abort_on_error=1\")\n";
    cmake_config += "endif()\n";

    return cmake_config;
}

std::string SecurityConfigManager::generateLinkerSecurityFlags(const SecurityConfig& config) const {
    std::string flags;

    for (const auto& flag : config.securityFlags) {
        std::string flag_str = getSecurityFlagString(flag, "gcc");
        if (flag_str.find("-Wl,") == 0 || flag_str == "-fPIE") {
            if (!flags.empty()) flags += " ";
            flags += flag_str;
        }
    }

    return flags;
}

std::string SecurityConfigManager::generateGitHubActionsSecurityConfig(const SecurityConfig& config) const {
    std::string yaml_config = "# Security scanning workflow\n";
    yaml_config += "name: Security Scan\n\n";
    yaml_config += "on:\n";
    yaml_config += "  push:\n";
    yaml_config += "    branches: [ main, develop ]\n";
    yaml_config += "  pull_request:\n";
    yaml_config += "    branches: [ main ]\n\n";

    yaml_config += "jobs:\n";
    yaml_config += "  security-scan:\n";
    yaml_config += "    runs-on: ubuntu-latest\n";
    yaml_config += "    steps:\n";
    yaml_config += "    - uses: actions/checkout@v3\n\n";

    // Static analysis steps
    if (!config.staticAnalyzers.empty()) {
        for (const auto& analyzer : config.staticAnalyzers) {
            if (analyzer == StaticAnalyzer::CppCheck) {
                yaml_config += "    - name: Run Cppcheck\n";
                yaml_config += "      run: |\n";
                yaml_config += "        sudo apt-get install cppcheck\n";
                yaml_config += "        cppcheck --enable=all --std=c++17 --error-exitcode=1 src/\n\n";
            } else if (analyzer == StaticAnalyzer::ClangStaticAnalyzer) {
                yaml_config += "    - name: Run Clang Static Analyzer\n";
                yaml_config += "      run: |\n";
                yaml_config += "        sudo apt-get install clang-tools\n";
                yaml_config += "        scan-build cmake -B build\n";
                yaml_config += "        scan-build -o scan-results cmake --build build\n\n";
            }
        }
    }

    // Dependency scanning
    if (config.enableDependencyScanning) {
        yaml_config += "    - name: Run dependency check\n";
        yaml_config += "      uses: dependency-check/Dependency-Check_Action@main\n";
        yaml_config += "      with:\n";
        yaml_config += "        project: 'cpp-project'\n";
        yaml_config += "        path: '.'\n";
        yaml_config += "        format: 'ALL'\n\n";
    }

    // Secrets scanning
    if (config.scanForSecrets) {
        yaml_config += "    - name: Run secrets scan\n";
        yaml_config += "      uses: trufflesecurity/trufflehog@main\n";
        yaml_config += "      with:\n";
        yaml_config += "        path: ./\n";
        yaml_config += "        base: main\n";
        yaml_config += "        head: HEAD\n\n";
    }

    return yaml_config;
}

std::string SecurityConfigManager::generateGitLabCISecurityConfig(const SecurityConfig& config) const {
    std::string yaml_config = "# Security scanning configuration for GitLab CI\n";
    yaml_config += "include:\n";
    yaml_config += "  - template: Security/SAST.gitlab-ci.yml\n";
    yaml_config += "  - template: Security/Dependency-Scanning.gitlab-ci.yml\n";
    yaml_config += "  - template: Security/Secret-Detection.gitlab-ci.yml\n\n";

    yaml_config += "variables:\n";
    yaml_config += "  SAST_EXCLUDED_PATHS: \"build/, tests/\"\n\n";

    // Custom static analysis job
    if (!config.staticAnalyzers.empty()) {
        yaml_config += "static-analysis:\n";
        yaml_config += "  stage: test\n";
        yaml_config += "  image: ubuntu:latest\n";
        yaml_config += "  before_script:\n";
        yaml_config += "    - apt-get update -qq && apt-get install -y -qq\n";

        for (const auto& analyzer : config.staticAnalyzers) {
            if (analyzer == StaticAnalyzer::CppCheck) {
                yaml_config += "    - apt-get install -y cppcheck\n";
            } else if (analyzer == StaticAnalyzer::ClangStaticAnalyzer) {
                yaml_config += "    - apt-get install -y clang-tools\n";
            }
        }

        yaml_config += "  script:\n";
        for (const auto& analyzer : config.staticAnalyzers) {
            if (analyzer == StaticAnalyzer::CppCheck) {
                yaml_config += "    - cppcheck --enable=all --std=c++17 --error-exitcode=1 src/\n";
            }
        }
        yaml_config += "  artifacts:\n";
        yaml_config += "    reports:\n";
        yaml_config += "      junit: cppcheck-report.xml\n\n";
    }

    return yaml_config;
}

std::string SecurityConfigManager::generateSecurityDocumentation(const SecurityConfig& config) const {
    std::string doc = "# Security Configuration\n\n";

    doc += fmt::format("**Security Level**: {}\n\n", to_string(config.level));

    // Static analyzers
    if (!config.staticAnalyzers.empty()) {
        doc += "## Static Analysis Tools\n\n";
        for (const auto& analyzer : config.staticAnalyzers) {
            doc += fmt::format("- **{}**: {}\n", to_string(analyzer), getAnalyzerDescription(analyzer));
        }
        doc += "\n";
    }

    // Sanitizers
    if (!config.sanitizers.empty()) {
        doc += "## Runtime Sanitizers\n\n";
        for (const auto& sanitizer : config.sanitizers) {
            doc += fmt::format("- **{}**: {}\n", to_string(sanitizer), getSanitizerDescription(sanitizer));
        }
        doc += "\n";
    }

    // Security flags
    if (!config.securityFlags.empty()) {
        doc += "## Security Hardening Flags\n\n";
        for (const auto& flag : config.securityFlags) {
            doc += fmt::format("- **{}**: {}\n", to_string(flag), getSecurityFlagDescription(flag));
        }
        doc += "\n";
    }

    // Additional security features
    doc += "## Additional Security Features\n\n";
    if (config.enableDependencyScanning) {
        doc += "- **Dependency Scanning**: Enabled\n";
    }
    if (config.enableVulnerabilityChecks) {
        doc += "- **Vulnerability Checks**: Enabled\n";
    }
    if (config.scanForSecrets) {
        doc += "- **Secrets Scanning**: Enabled\n";
    }
    if (config.enableFuzzTesting) {
        doc += "- **Fuzz Testing**: Enabled\n";
    }

    doc += "\n## Build Instructions\n\n";
    doc += "To build with security features enabled:\n\n";
    doc += "```bash\n";
    doc += "mkdir build && cd build\n";
    doc += "cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON\n";
    doc += "cmake --build .\n";
    doc += "```\n\n";

    return doc;
}

std::string SecurityConfigManager::generateSecurityChecklist(const SecurityConfig& config) const {
    std::string checklist = "# Security Checklist\n\n";

    checklist += "## Pre-Development\n\n";
    checklist += "- [ ] Security requirements defined\n";
    checklist += "- [ ] Threat model created\n";
    checklist += "- [ ] Security tools configured\n\n";

    checklist += "## Development\n\n";
    checklist += "- [ ] Static analysis tools integrated\n";
    checklist += "- [ ] Sanitizers enabled for debug builds\n";
    checklist += "- [ ] Security flags enabled\n";
    checklist += "- [ ] Input validation implemented\n";
    checklist += "- [ ] Error handling reviewed\n";
    checklist += "- [ ] Memory management audited\n\n";

    checklist += "## Testing\n\n";
    checklist += "- [ ] Unit tests include security test cases\n";
    checklist += "- [ ] Integration tests cover security scenarios\n";
    if (config.enableFuzzTesting) {
        checklist += "- [ ] Fuzz testing implemented\n";
    }
    checklist += "- [ ] Penetration testing performed\n\n";

    checklist += "## Deployment\n\n";
    checklist += "- [ ] Dependencies scanned for vulnerabilities\n";
    checklist += "- [ ] Secrets removed from code\n";
    checklist += "- [ ] Security headers configured\n";
    checklist += "- [ ] Access controls implemented\n";
    checklist += "- [ ] Monitoring and logging enabled\n\n";

    checklist += "## Maintenance\n\n";
    checklist += "- [ ] Regular security updates applied\n";
    checklist += "- [ ] Vulnerability scanning automated\n";
    checklist += "- [ ] Security incidents documented\n";
    checklist += "- [ ] Security training completed\n\n";

    return checklist;
}

std::string SecurityConfigManager::getAnalyzerCommand(StaticAnalyzer analyzer) const {
    auto it = analyzerCommands_.find(analyzer);
    return it != analyzerCommands_.end() ? it->second : "";
}

std::string SecurityConfigManager::getAnalyzerDescription(StaticAnalyzer analyzer) const {
    switch (analyzer) {
        case StaticAnalyzer::ClangStaticAnalyzer:
            return "Clang's built-in static analyzer for finding bugs and security issues";
        case StaticAnalyzer::CppCheck:
            return "Static analysis tool for C/C++ code to detect bugs and undefined behavior";
        case StaticAnalyzer::PVSStudio:
            return "Commercial static analyzer with advanced bug detection capabilities";
        case StaticAnalyzer::SonarQube:
            return "Platform for continuous code quality and security analysis";
        case StaticAnalyzer::Clazy:
            return "Qt-oriented static analyzer based on Clang";
        case StaticAnalyzer::IncludeWhatYouUse:
            return "Tool for analyzing #includes in C/C++ source files";
        default:
            return "Unknown analyzer";
    }
}

std::string SecurityConfigManager::getSanitizerDescription(Sanitizer sanitizer) const {
    switch (sanitizer) {
        case Sanitizer::AddressSanitizer:
            return "Detects buffer overflows, use-after-free, and other memory errors";
        case Sanitizer::ThreadSanitizer:
            return "Detects data races and other thread safety issues";
        case Sanitizer::UndefinedBehaviorSanitizer:
            return "Detects undefined behavior at runtime";
        case Sanitizer::MemorySanitizer:
            return "Detects reads of uninitialized memory";
        case Sanitizer::LeakSanitizer:
            return "Detects memory leaks";
        case Sanitizer::ControlFlowIntegrity:
            return "Detects control flow hijacking attacks";
        default:
            return "Unknown sanitizer";
    }
}

std::string SecurityConfigManager::getSecurityFlagDescription(SecurityFlag flag) const {
    switch (flag) {
        case SecurityFlag::StackProtector:
            return "Protects against stack buffer overflow attacks";
        case SecurityFlag::Fortify:
            return "Compile-time and runtime protection against buffer overflows";
        case SecurityFlag::RelRO:
            return "Makes some sections of the binary read-only after relocation";
        case SecurityFlag::NowBinding:
            return "Resolves all symbols at program startup";
        case SecurityFlag::NoExecutableStack:
            return "Marks the stack as non-executable";
        case SecurityFlag::PieExecutable:
            return "Creates position-independent executable for ASLR";
        case SecurityFlag::ControlFlowGuard:
            return "Microsoft's control flow integrity protection";
        case SecurityFlag::ShadowStack:
            return "Intel CET shadow stack protection";
        default:
            return "Unknown security flag";
    }
}

// Utility functions implementation
std::string to_string(SecurityLevel level) {
    switch (level) {
        case SecurityLevel::None: return "none";
        case SecurityLevel::Basic: return "basic";
        case SecurityLevel::Enhanced: return "enhanced";
        case SecurityLevel::Paranoid: return "paranoid";
        default: return "unknown";
    }
}

std::string to_string(StaticAnalyzer analyzer) {
    switch (analyzer) {
        case StaticAnalyzer::None: return "none";
        case StaticAnalyzer::ClangStaticAnalyzer: return "clang-static-analyzer";
        case StaticAnalyzer::CppCheck: return "cppcheck";
        case StaticAnalyzer::PVSStudio: return "pvs-studio";
        case StaticAnalyzer::SonarQube: return "sonarqube";
        case StaticAnalyzer::Clazy: return "clazy";
        case StaticAnalyzer::IncludeWhatYouUse: return "include-what-you-use";
        default: return "unknown";
    }
}

std::string to_string(Sanitizer sanitizer) {
    switch (sanitizer) {
        case Sanitizer::None: return "none";
        case Sanitizer::AddressSanitizer: return "address";
        case Sanitizer::ThreadSanitizer: return "thread";
        case Sanitizer::UndefinedBehaviorSanitizer: return "undefined";
        case Sanitizer::MemorySanitizer: return "memory";
        case Sanitizer::LeakSanitizer: return "leak";
        case Sanitizer::ControlFlowIntegrity: return "cfi";
        default: return "unknown";
    }
}

std::string to_string(SecurityFlag flag) {
    switch (flag) {
        case SecurityFlag::StackProtector: return "stack-protector";
        case SecurityFlag::Fortify: return "fortify";
        case SecurityFlag::RelRO: return "relro";
        case SecurityFlag::NowBinding: return "now-binding";
        case SecurityFlag::NoExecutableStack: return "no-exec-stack";
        case SecurityFlag::PieExecutable: return "pie";
        case SecurityFlag::ControlFlowGuard: return "control-flow-guard";
        case SecurityFlag::ShadowStack: return "shadow-stack";
        case SecurityFlag::ReturnAddressAuth: return "return-address-auth";
        case SecurityFlag::BranchTargetId: return "branch-target-id";
        default: return "unknown";
    }
}

std::optional<SecurityLevel> to_security_level(const std::string& str) {
    static const std::unordered_map<std::string, SecurityLevel> map = {
        {"none", SecurityLevel::None},
        {"basic", SecurityLevel::Basic},
        {"enhanced", SecurityLevel::Enhanced},
        {"paranoid", SecurityLevel::Paranoid}
    };

    auto it = map.find(str);
    return it != map.end() ? std::optional<SecurityLevel>{it->second} : std::nullopt;
}

std::optional<StaticAnalyzer> to_static_analyzer(const std::string& str) {
    static const std::unordered_map<std::string, StaticAnalyzer> map = {
        {"none", StaticAnalyzer::None},
        {"clang-static-analyzer", StaticAnalyzer::ClangStaticAnalyzer},
        {"cppcheck", StaticAnalyzer::CppCheck},
        {"pvs-studio", StaticAnalyzer::PVSStudio},
        {"sonarqube", StaticAnalyzer::SonarQube},
        {"clazy", StaticAnalyzer::Clazy},
        {"include-what-you-use", StaticAnalyzer::IncludeWhatYouUse}
    };

    auto it = map.find(str);
    return it != map.end() ? std::optional<StaticAnalyzer>{it->second} : std::nullopt;
}

std::optional<Sanitizer> to_sanitizer(const std::string& str) {
    static const std::unordered_map<std::string, Sanitizer> map = {
        {"none", Sanitizer::None},
        {"address", Sanitizer::AddressSanitizer},
        {"thread", Sanitizer::ThreadSanitizer},
        {"undefined", Sanitizer::UndefinedBehaviorSanitizer},
        {"memory", Sanitizer::MemorySanitizer},
        {"leak", Sanitizer::LeakSanitizer},
        {"cfi", Sanitizer::ControlFlowIntegrity}
    };

    auto it = map.find(str);
    return it != map.end() ? std::optional<Sanitizer>{it->second} : std::nullopt;
}

std::optional<SecurityFlag> to_security_flag(const std::string& str) {
    static const std::unordered_map<std::string, SecurityFlag> map = {
        {"stack-protector", SecurityFlag::StackProtector},
        {"fortify", SecurityFlag::Fortify},
        {"relro", SecurityFlag::RelRO},
        {"now-binding", SecurityFlag::NowBinding},
        {"no-exec-stack", SecurityFlag::NoExecutableStack},
        {"pie", SecurityFlag::PieExecutable},
        {"control-flow-guard", SecurityFlag::ControlFlowGuard},
        {"shadow-stack", SecurityFlag::ShadowStack},
        {"return-address-auth", SecurityFlag::ReturnAddressAuth},
        {"branch-target-id", SecurityFlag::BranchTargetId}
    };

    auto it = map.find(str);
    return it != map.end() ? std::optional<SecurityFlag>{it->second} : std::nullopt;
}

} // namespace utils
