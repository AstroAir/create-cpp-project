#include "build_system_config.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <sstream>

#include "file_utils.h"
#include "terminal_utils.h"

namespace utils {

BuildSystemConfigManager& BuildSystemConfigManager::getInstance() {
    static BuildSystemConfigManager instance;
    static bool initialized = false;

    if (!initialized) {
        instance.initializeDefaultConfigs();
        instance.loadCompilerDatabase();
        instance.loadPlatformDatabase();
        initialized = true;
    }

    return instance;
}

BuildSystemConfig BuildSystemConfigManager::createDefaultConfig(BuildSystem buildSystem,
                                                                TemplateType templateType) {
    BuildSystemConfig config;
    config.buildSystem = buildSystem;
    config.cppStandard = CppStandard::Cpp17;
    config.optimizationLevel = OptimizationLevel::Speed;
    config.buildConfiguration = BuildConfiguration::Release;

    // Set template-specific defaults
    switch (templateType) {
        case TemplateType::GameEngine:
            config.cppStandard = CppStandard::Cpp20;
            config.optimizationLevel = OptimizationLevel::Aggressive;
            config.enableProfiling = true;
            config.compilerSettings.enableLTO = true;
            break;

        case TemplateType::Embedded:
            config.optimizationLevel = OptimizationLevel::Size;
            config.staticLinking = true;
            config.sharedLibraries = false;
            break;

        case TemplateType::Lib:
        case TemplateType::HeaderOnlyLib:
            config.enableExamples = true;
            config.enableBenchmarking = true;
            config.enableInstall = true;
            config.enablePackaging = true;
            break;

        default:
            // Use defaults
            break;
    }

    // Set build system specific defaults
    switch (buildSystem) {
        case BuildSystem::CMake:
            config.compilerSettings.enableCcache = true;
            config.enableTesting = true;
            break;

        case BuildSystem::Bazel:
            config.compilerSettings.enableLTO = true;
            config.enableDistributedBuild = true;
            break;

        case BuildSystem::Meson:
            config.compilerSettings.enablePCH = true;
            config.enableBenchmarking = true;
            break;

        default:
            break;
    }

    return config;
}

BuildSystemConfig BuildSystemConfigManager::configureInteractively(const CliOptions& options) {
    BuildSystemConfig config = createDefaultConfig(options.buildSystem, options.templateType);

    TerminalUtils::showNpmStyleHeader("Build System Configuration");

    // C++ Standard selection
    config.cppStandard = selectCppStandard(config.cppStandard);

    // Optimization level selection
    config.optimizationLevel = selectOptimizationLevel(config.optimizationLevel);

    // Advanced options
    std::vector<std::string> advancedOptions = {"Enable Link-Time Optimization (LTO)",
                                                "Enable Precompiled Headers",
                                                "Enable ccache",
                                                "Enable static analysis (clang-tidy)",
                                                "Enable sanitizers",
                                                "Enable code coverage",
                                                "Enable profiling support"};

    std::vector<bool> defaultSelected = {config.compilerSettings.enableLTO,
                                         config.compilerSettings.enablePCH,
                                         config.compilerSettings.enableCcache,
                                         config.compilerSettings.enableClangTidy,
                                         config.enableSanitizers,
                                         config.enableCoverage,
                                         config.enableProfiling};

    auto selectedOptions = TerminalUtils::showMultiSelectDialog("Select advanced build options",
                                                                advancedOptions, defaultSelected);

    // Apply selected options
    config.compilerSettings.enableLTO =
            std::find(selectedOptions.begin(), selectedOptions.end(),
                      "Enable Link-Time Optimization (LTO)") != selectedOptions.end();
    config.compilerSettings.enablePCH =
            std::find(selectedOptions.begin(), selectedOptions.end(),
                      "Enable Precompiled Headers") != selectedOptions.end();
    config.compilerSettings.enableCcache = std::find(selectedOptions.begin(), selectedOptions.end(),
                                                     "Enable ccache") != selectedOptions.end();
    config.compilerSettings.enableClangTidy =
            std::find(selectedOptions.begin(), selectedOptions.end(),
                      "Enable static analysis (clang-tidy)") != selectedOptions.end();
    config.enableSanitizers = std::find(selectedOptions.begin(), selectedOptions.end(),
                                        "Enable sanitizers") != selectedOptions.end();
    config.enableCoverage = std::find(selectedOptions.begin(), selectedOptions.end(),
                                      "Enable code coverage") != selectedOptions.end();
    config.enableProfiling = std::find(selectedOptions.begin(), selectedOptions.end(),
                                       "Enable profiling support") != selectedOptions.end();

    TerminalUtils::showNpmStyleSuccess("Build configuration completed");

    return config;
}

CppStandard BuildSystemConfigManager::selectCppStandard(CppStandard defaultStandard) {
    std::vector<std::string> standards = {"C++11", "C++14", "C++17", "C++20", "C++23", "Latest"};

    int defaultIndex = static_cast<int>(defaultStandard);

    int selected =
            TerminalUtils::showInteractiveMenu(standards, "Select C++ standard", defaultIndex);

    if (selected >= 0 && selected < static_cast<int>(standards.size())) {
        TerminalUtils::showNpmStyleSuccess("C++ standard selected", standards[selected]);
        return static_cast<CppStandard>(selected);
    }

    return defaultStandard;
}

OptimizationLevel BuildSystemConfigManager::selectOptimizationLevel(
        OptimizationLevel defaultLevel) {
    std::vector<std::string> levels = {
            "None (-O0) - No optimization, fastest compilation",
            "Debug (-Og) - Optimize for debugging", "Size (-Os) - Optimize for size",
            "Speed (-O2) - Optimize for speed", "Aggressive (-O3) - Maximum optimization"};

    int defaultIndex = static_cast<int>(defaultLevel);

    int selected =
            TerminalUtils::showInteractiveMenu(levels, "Select optimization level", defaultIndex);

    if (selected >= 0 && selected < static_cast<int>(levels.size())) {
        std::string levelName = levels[selected].substr(0, levels[selected].find(' '));
        TerminalUtils::showNpmStyleSuccess("Optimization level selected", levelName);
        return static_cast<OptimizationLevel>(selected);
    }

    return defaultLevel;
}

std::string BuildSystemConfigManager::generateCMakeConfig(const BuildSystemConfig& config,
                                                          const std::string& projectName) {
    std::ostringstream cmake;

    cmake << "cmake_minimum_required(VERSION 3.14)\n";
    cmake << fmt::format("project({} VERSION 1.0.0 LANGUAGES CXX)\n\n", projectName);

    // C++ standard
    std::string cppStd = cppStandardToString(config.cppStandard);
    cmake << fmt::format("set(CMAKE_CXX_STANDARD {})\n", cppStd);
    cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    cmake << "set(CMAKE_CXX_EXTENSIONS OFF)\n\n";

    // Build type
    cmake << "if(NOT CMAKE_BUILD_TYPE)\n";
    cmake << fmt::format("    set(CMAKE_BUILD_TYPE {})\n",
                         buildConfigurationToString(config.buildConfiguration));
    cmake << "endif()\n\n";

    // Compiler flags
    if (!config.compilerSettings.cxxFlags.empty()) {
        cmake << "# Compiler flags\n";
        for (const auto& flag : config.compilerSettings.cxxFlags) {
            cmake << fmt::format("add_compile_options({})\n", flag);
        }
        cmake << "\n";
    }

    // LTO support
    if (config.compilerSettings.enableLTO) {
        cmake << "# Link-Time Optimization\n";
        cmake << "include(CheckIPOSupported)\n";
        cmake << "check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)\n";
        cmake << "if(ipo_supported)\n";
        cmake << "    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)\n";
        cmake << "endif()\n\n";
    }

    // ccache support
    if (config.compilerSettings.enableCcache) {
        cmake << "# ccache support\n";
        cmake << "find_program(CCACHE_PROGRAM ccache)\n";
        cmake << "if(CCACHE_PROGRAM)\n";
        cmake << "    set(CMAKE_CXX_COMPILER_LAUNCHER \"${CCACHE_PROGRAM}\")\n";
        cmake << "endif()\n\n";
    }

    // Testing
    if (config.enableTesting) {
        cmake << "# Testing\n";
        cmake << "option(BUILD_TESTING \"Build tests\" ON)\n";
        cmake << "if(BUILD_TESTING)\n";
        cmake << "    enable_testing()\n";
        cmake << "endif()\n\n";
    }

    // Coverage
    if (config.enableCoverage) {
        cmake << "# Code coverage\n";
        cmake << "option(ENABLE_COVERAGE \"Enable code coverage\" OFF)\n";
        cmake << "if(ENABLE_COVERAGE)\n";
        cmake << "    add_compile_options(--coverage)\n";
        cmake << "    add_link_options(--coverage)\n";
        cmake << "endif()\n\n";
    }

    // Sanitizers
    if (config.enableSanitizers) {
        cmake << "# Sanitizers\n";
        cmake << "option(ENABLE_SANITIZERS \"Enable sanitizers\" OFF)\n";
        cmake << "if(ENABLE_SANITIZERS)\n";
        cmake << "    add_compile_options(-fsanitize=address,undefined)\n";
        cmake << "    add_link_options(-fsanitize=address,undefined)\n";
        cmake << "endif()\n\n";
    }

    // Source files placeholder
    cmake << "# Source files\n";
    cmake << "set(SOURCES\n";
    cmake << "    src/main.cpp\n";
    cmake << "    # Add your source files here\n";
    cmake << ")\n\n";

    // Executable/Library target
    cmake << fmt::format("add_executable({} ${{SOURCES}})\n\n", projectName);

    // Install rules
    if (config.enableInstall) {
        cmake << "# Install rules\n";
        cmake << fmt::format("install(TARGETS {} DESTINATION bin)\n", projectName);
    }

    return cmake.str();
}

std::string BuildSystemConfigManager::generateMesonConfig(const BuildSystemConfig& config,
                                                          const std::string& projectName) {
    std::ostringstream meson;

    std::string cppStd = "cpp_std=c++" + cppStandardToString(config.cppStandard);

    meson << fmt::format("project('{}', 'cpp',\n", projectName);
    meson << "  version : '1.0.0',\n";
    meson << fmt::format("  default_options : ['warning_level=3', '{}'])\n\n", cppStd);

    // Dependencies
    meson << "# Dependencies\n";
    meson << "# Add your dependencies here\n\n";

    // Source files
    meson << "# Source files\n";
    meson << "sources = files(\n";
    meson << "  'src/main.cpp',\n";
    meson << "  # Add your source files here\n";
    meson << ")\n\n";

    // Executable
    meson << fmt::format("executable('{}', sources,\n", projectName);
    meson << "  install : true)\n\n";

    // Testing
    if (config.enableTesting) {
        meson << "# Testing\n";
        meson << "if get_option('tests')\n";
        meson << "  subdir('tests')\n";
        meson << "endif\n";
    }

    return meson.str();
}

void BuildSystemConfigManager::initializeDefaultConfigs() {
    // Initialize default configurations for each build system
    for (auto buildSystem : {BuildSystem::CMake, BuildSystem::Meson, BuildSystem::Bazel}) {
        BuildSystemConfig config;
        config.buildSystem = buildSystem;
        config.cppStandard = CppStandard::Cpp17;
        config.optimizationLevel = OptimizationLevel::Speed;
        config.buildConfiguration = BuildConfiguration::Release;

        defaultConfigs_[buildSystem] = config;
    }
}

void BuildSystemConfigManager::loadCompilerDatabase() {
    // Load compiler-specific settings
    CompilerSettings gccSettings;
    gccSettings.warningFlags = {"-Wall", "-Wextra", "-Wpedantic"};
    gccSettings.cxxFlags = {"-fdiagnostics-color=always"};
    compilerDatabase_["gcc"] = gccSettings;

    CompilerSettings clangSettings;
    clangSettings.warningFlags = {"-Wall", "-Wextra", "-Wpedantic"};
    clangSettings.cxxFlags = {"-fcolor-diagnostics"};
    compilerDatabase_["clang"] = clangSettings;

    CompilerSettings msvcSettings;
    msvcSettings.warningFlags = {"/W4"};
    msvcSettings.cxxFlags = {"/permissive-"};
    compilerDatabase_["msvc"] = msvcSettings;
}

void BuildSystemConfigManager::loadPlatformDatabase() {
    // Load platform-specific settings
    PlatformSettings linuxSettings;
    linuxSettings.systemLibraries = {"pthread", "dl"};
    platformDatabase_["linux"] = linuxSettings;

    PlatformSettings windowsSettings;
    windowsSettings.systemLibraries = {"kernel32", "user32"};
    platformDatabase_["windows"] = windowsSettings;

    PlatformSettings macosSettings;
    macosSettings.frameworks = {"Foundation", "CoreFoundation"};
    platformDatabase_["macos"] = macosSettings;
}

std::string BuildSystemConfigManager::cppStandardToString(CppStandard standard) {
    switch (standard) {
        case CppStandard::Cpp11:
            return "11";
        case CppStandard::Cpp14:
            return "14";
        case CppStandard::Cpp17:
            return "17";
        case CppStandard::Cpp20:
            return "20";
        case CppStandard::Cpp23:
            return "23";
        case CppStandard::Latest:
            return "23";
        default:
            return "17";
    }
}

std::string BuildSystemConfigManager::buildConfigurationToString(BuildConfiguration config) {
    switch (config) {
        case BuildConfiguration::Debug:
            return "Debug";
        case BuildConfiguration::Release:
            return "Release";
        case BuildConfiguration::RelWithDebInfo:
            return "RelWithDebInfo";
        case BuildConfiguration::MinSizeRel:
            return "MinSizeRel";
        case BuildConfiguration::Custom:
            return "Custom";
        default:
            return "Release";
    }
}

}  // namespace utils
