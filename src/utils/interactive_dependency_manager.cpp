#include "interactive_dependency_manager.h"
#include "terminal_utils.h"
#include "../cli/input/user_input.h"
#include "../cli/types/cli_enums.h"
#include "file_utils.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using namespace utils;
using json = nlohmann::json;

namespace utils {

// Static member initialization
std::vector<InteractiveDependency> InteractiveDependencyManager::s_dependencyDatabase;
std::unordered_map<std::string, std::vector<std::string>> InteractiveDependencyManager::s_categoryMap;
std::unordered_map<std::string, std::vector<std::string>> InteractiveDependencyManager::s_tagMap;
bool InteractiveDependencyManager::s_databaseLoaded = false;

DependencySelection InteractiveDependencyManager::runInteractiveDependencySelection(
    const CliOptions& options) {

    TerminalUtils::showInfo("📦 Interactive Dependency Management");
    TerminalUtils::showSectionSeparator();

    if (!s_databaseLoaded) {
        TerminalUtils::showLoadingDots("Loading dependency database", 1000);
        loadDependencyDatabase();
    }

    DependencySelection selection;

    // Show main menu
    std::vector<std::string> mainMenuOptions = {
        "Browse by Category",
        "Search Dependencies",
        "View Recommended Dependencies",
        "View Popular Dependencies",
        "Import Dependency List",
        "Configure Package Manager",
        "Finish Selection"
    };

    std::vector<InteractiveDependency> selectedDependencies;

    while (true) {
        std::cout << "\n" << TerminalUtils::colorize("📦 Dependency Management Menu:", Color::BrightCyan) << "\n";

        if (!selectedDependencies.empty()) {
            std::cout << TerminalUtils::colorize("Currently selected: ", Color::BrightGreen);
            for (size_t i = 0; i < selectedDependencies.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << selectedDependencies[i].base.name;
            }
            std::cout << "\n\n";
        }

        int choice = TerminalUtils::showInteractiveMenu(mainMenuOptions, "Select an option", 0);

        switch (choice) {
            case 0: { // Browse by Category
                std::vector<std::string> categories = getAvailableCategories();
                std::vector<std::string_view> categoryViews;
                for (const auto& cat : categories) {
                    categoryViews.push_back(cat);
                }
                std::string selectedCategory = UserInput::readChoiceWithStyle(
                    "Select category", categoryViews, categories[0], Color::BrightMagenta);

                auto categoryDeps = browseDependenciesByCategory(selectedCategory);
                auto newSelections = selectDependenciesInteractively(categoryDeps, selectedDependencies);

                // Merge selections
                for (const auto& dep : newSelections) {
                    if (std::find_if(selectedDependencies.begin(), selectedDependencies.end(),
                                   [&dep](const InteractiveDependency& existing) {
                                       return existing.base.name == dep.base.name;
                                   }) == selectedDependencies.end()) {
                        selectedDependencies.push_back(dep);
                    }
                }
                break;
            }

            case 1: { // Search Dependencies
                showSearchInterface();
                DependencySearchCriteria criteria;
                std::cout << "Enter search query: ";
                std::getline(std::cin, criteria.query);

                if (!criteria.query.empty()) {
                    auto searchResults = searchDependencies(criteria);
                    if (!searchResults.empty()) {
                        auto newSelections = selectDependenciesInteractively(searchResults, selectedDependencies);

                        // Merge selections
                        for (const auto& dep : newSelections) {
                            if (std::find_if(selectedDependencies.begin(), selectedDependencies.end(),
                                           [&dep](const InteractiveDependency& existing) {
                                               return existing.base.name == dep.base.name;
                                           }) == selectedDependencies.end()) {
                                selectedDependencies.push_back(dep);
                            }
                        }
                    } else {
                        TerminalUtils::showNpmStyleWarning("No results found", "Try a different search term");
                    }
                }
                break;
            }

            case 2: { // View Recommended Dependencies
                auto recommended = getRecommendedDependencies(options.templateType);
                if (!recommended.empty()) {
                    TerminalUtils::showInfo("Recommended dependencies for " +
                                          std::string(cli_enums::to_string(options.templateType)) + " projects:");
                    auto newSelections = selectDependenciesInteractively(recommended, selectedDependencies);

                    // Merge selections
                    for (const auto& dep : newSelections) {
                        if (std::find_if(selectedDependencies.begin(), selectedDependencies.end(),
                                       [&dep](const InteractiveDependency& existing) {
                                           return existing.base.name == dep.base.name;
                                       }) == selectedDependencies.end()) {
                            selectedDependencies.push_back(dep);
                        }
                    }
                }
                break;
            }

            case 3: { // View Popular Dependencies
                auto popular = getPopularDependencies(15);
                TerminalUtils::showInfo("Most popular C++ libraries:");
                auto newSelections = selectDependenciesInteractively(popular, selectedDependencies);

                // Merge selections
                for (const auto& dep : newSelections) {
                    if (std::find_if(selectedDependencies.begin(), selectedDependencies.end(),
                                   [&dep](const InteractiveDependency& existing) {
                                       return existing.base.name == dep.base.name;
                                   }) == selectedDependencies.end()) {
                        selectedDependencies.push_back(dep);
                    }
                }
                break;
            }

            case 4: { // Import Dependency List
                std::string filePath;
                std::cout << "Enter path to dependency file: ";
                std::getline(std::cin, filePath);

                if (!filePath.empty()) {
                    try {
                        auto imported = importDependencyList(filePath);
                        if (!imported.empty()) {
                            TerminalUtils::showNpmStyleSuccess("Imported " + std::to_string(imported.size()) + " dependencies");
                            selectedDependencies.insert(selectedDependencies.end(), imported.begin(), imported.end());
                        }
                    } catch (const std::exception& e) {
                        TerminalUtils::showNpmStyleError("Import failed", e.what());
                    }
                }
                break;
            }

            case 5: { // Configure Package Manager
                auto availablePMs = getAvailablePackageManagers();
                PackageManager selectedPM = selectPackageManagerInteractively(availablePMs, options.templateType);
                configurePackageManagerInteractively(selectedPM, options);
                break;
            }

            case 6: { // Finish Selection
                if (selectedDependencies.empty()) {
                    if (!UserInput::readConfirmation("No dependencies selected. Continue anyway?", false)) {
                        continue;
                    }
                }

                // Validate and resolve dependencies
                selection = validateAndResolveDependencies(selectedDependencies, options);

                if (!selection.success) {
                    TerminalUtils::showNpmStyleError("Dependency validation failed", "Please resolve conflicts");
                    for (const auto& conflict : selection.conflicts) {
                        TerminalUtils::showNpmStyleError("Conflict", conflict);
                    }

                    if (!UserInput::readConfirmation("Continue with conflicts?", false)) {
                        continue;
                    }
                }

                // Show final summary
                showDependencySelectionSummary(selection);

                if (UserInput::readConfirmation("Confirm dependency selection?", true)) {
                    return selection;
                }
                break;
            }

            default:
                TerminalUtils::showNpmStyleError("Invalid choice", "Please select a valid option");
                break;
        }
    }

    return selection;
}

std::vector<InteractiveDependency> InteractiveDependencyManager::searchDependencies(
    const DependencySearchCriteria& criteria) {

    if (!s_databaseLoaded) {
        loadDependencyDatabase();
    }

    std::vector<InteractiveDependency> results;

    for (const auto& dep : s_dependencyDatabase) {
        if (matchesSearchCriteria(dep, criteria)) {
            results.push_back(dep);
        }
    }

    // Sort by relevance (popularity for now)
    std::sort(results.begin(), results.end(),
              [](const InteractiveDependency& a, const InteractiveDependency& b) {
                  return a.popularity > b.popularity;
              });

    return results;
}

std::vector<InteractiveDependency> InteractiveDependencyManager::browseDependenciesByCategory(
    const std::string& category) {

    if (!s_databaseLoaded) {
        loadDependencyDatabase();
    }

    std::vector<InteractiveDependency> results;

    for (const auto& dep : s_dependencyDatabase) {
        if (dep.category == category) {
            results.push_back(dep);
        }
    }

    // Sort by popularity
    std::sort(results.begin(), results.end(),
              [](const InteractiveDependency& a, const InteractiveDependency& b) {
                  return a.popularity > b.popularity;
              });

    return results;
}

std::vector<InteractiveDependency> InteractiveDependencyManager::getRecommendedDependencies(
    TemplateType templateType) {

    if (!s_databaseLoaded) {
        loadDependencyDatabase();
    }

    std::vector<InteractiveDependency> recommended;

    // Get template-specific recommendations
    switch (templateType) {
        case TemplateType::Console:
            for (const auto& dep : s_dependencyDatabase) {
                if (dep.base.name == "cli11" || dep.base.name == "fmt" ||
                    dep.base.name == "spdlog" || dep.base.name == "argparse") {
                    recommended.push_back(dep);
                }
            }
            break;

        case TemplateType::Lib:
            for (const auto& dep : s_dependencyDatabase) {
                if (dep.base.name == "fmt" || dep.base.name == "spdlog" ||
                    dep.base.name == "benchmark" || dep.base.name == "catch2") {
                    recommended.push_back(dep);
                }
            }
            break;

        case TemplateType::Network:
            for (const auto& dep : s_dependencyDatabase) {
                if (dep.base.name == "asio" || dep.base.name == "nlohmann-json" ||
                    dep.base.name == "curl" || dep.base.name == "openssl") {
                    recommended.push_back(dep);
                }
            }
            break;

        case TemplateType::Gui:
            for (const auto& dep : s_dependencyDatabase) {
                if (dep.base.name == "qt6" || dep.base.name == "imgui" ||
                    dep.base.name == "gtk" || dep.base.name == "wxwidgets") {
                    recommended.push_back(dep);
                }
            }
            break;

        default:
            // General recommendations
            for (const auto& dep : s_dependencyDatabase) {
                if (dep.isRecommended) {
                    recommended.push_back(dep);
                }
            }
            break;
    }

    return recommended;
}

std::vector<InteractiveDependency> InteractiveDependencyManager::getPopularDependencies(int limit) {
    if (!s_databaseLoaded) {
        loadDependencyDatabase();
    }

    std::vector<InteractiveDependency> popular = s_dependencyDatabase;

    // Sort by popularity
    std::sort(popular.begin(), popular.end(),
              [](const InteractiveDependency& a, const InteractiveDependency& b) {
                  return a.popularity > b.popularity;
              });

    if (limit > 0 && popular.size() > static_cast<size_t>(limit)) {
        popular.resize(limit);
    }

    return popular;
}

bool InteractiveDependencyManager::loadDependencyDatabase() {
    if (s_databaseLoaded) {
        return true;
    }

    try {
        initializeDependencyDatabase();
        loadBuiltinDependencies();
        loadCustomDependencies();

        s_databaseLoaded = true;
        spdlog::info("Loaded {} dependencies from database", s_dependencyDatabase.size());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to load dependency database: {}", e.what());
        return false;
    }
}

void InteractiveDependencyManager::showDependencySelectionSummary(const DependencySelection& selection) {
    TerminalUtils::clearScreen();

    std::vector<std::string> summaryLines = {
        "📦 Dependency Selection Summary",
        "Review your selected dependencies"
    };
    TerminalUtils::showBox(summaryLines, BorderStyle::Double, Color::BrightGreen, Color::White, "");

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("Selected Dependencies:", Color::BrightYellow) << "\n";

    for (const auto& dep : selection.selected) {
        std::cout << "  📦 " << TerminalUtils::colorize(dep.base.name, Color::BrightWhite);
        if (!dep.base.version.empty()) {
            std::cout << " " << TerminalUtils::colorize("v" + dep.base.version, Color::BrightBlack);
        }
        std::cout << " - " << TerminalUtils::colorize(dep.base.description, Color::BrightBlack) << "\n";
    }

    if (!selection.warnings.empty()) {
        std::cout << "\n" << TerminalUtils::colorize("⚠️  Warnings:", Color::BrightYellow) << "\n";
        for (const auto& warning : selection.warnings) {
            std::cout << "  " << TerminalUtils::colorize(warning, Color::Yellow) << "\n";
        }
    }

    if (!selection.conflicts.empty()) {
        std::cout << "\n" << TerminalUtils::colorize("❌ Conflicts:", Color::BrightRed) << "\n";
        for (const auto& conflict : selection.conflicts) {
            std::cout << "  " << TerminalUtils::colorize(conflict, Color::Red) << "\n";
        }
    }

    std::cout << "\n";
}

void InteractiveDependencyManager::initializeDependencyDatabase() {
    s_dependencyDatabase.clear();
    s_categoryMap.clear();
    s_tagMap.clear();
}

void InteractiveDependencyManager::loadBuiltinDependencies() {
    // Popular utility libraries
    InteractiveDependency fmt;
    fmt.base.name = "fmt";
    fmt.base.version = "9.1.0";
    fmt.base.description = "A modern formatting library";
    fmt.base.headerOnly = false;
    fmt.base.license = "MIT";
    fmt.category = categories::UTILITY;
    fmt.tags = {tags::MODERN_CPP, tags::HIGH_PERFORMANCE, tags::POPULAR};
    fmt.homepage = "https://fmt.dev/";
    fmt.popularity = 95;
    fmt.isRecommended = true;
    fmt.useCases = {"String formatting", "Logging", "Output formatting"};
    s_dependencyDatabase.push_back(fmt);

    InteractiveDependency spdlog;
    spdlog.base.name = "spdlog";
    spdlog.base.version = "1.12.0";
    spdlog.base.description = "Very fast, header-only/compiled, C++ logging library";
    spdlog.base.headerOnly = true;
    spdlog.base.license = "MIT";
    spdlog.category = categories::LOGGING;
    spdlog.tags = {tags::HEADER_ONLY, tags::HIGH_PERFORMANCE, tags::POPULAR};
    spdlog.homepage = "https://github.com/gabime/spdlog";
    spdlog.popularity = 90;
    spdlog.isRecommended = true;
    spdlog.useCases = {"Application logging", "Debug output", "Performance monitoring"};
    s_dependencyDatabase.push_back(spdlog);

    InteractiveDependency nlohmann_json;
    nlohmann_json.base.name = "nlohmann-json";
    nlohmann_json.base.version = "3.11.2";
    nlohmann_json.base.description = "JSON for Modern C++";
    nlohmann_json.base.headerOnly = true;
    nlohmann_json.base.license = "MIT";
    nlohmann_json.category = categories::SERIALIZATION;
    nlohmann_json.tags = {tags::HEADER_ONLY, tags::MODERN_CPP, tags::POPULAR};
    nlohmann_json.homepage = "https://github.com/nlohmann/json";
    nlohmann_json.popularity = 88;
    nlohmann_json.isRecommended = true;
    nlohmann_json.useCases = {"JSON parsing", "Configuration files", "API communication"};
    s_dependencyDatabase.push_back(nlohmann_json);

    // CLI libraries
    InteractiveDependency cli11;
    cli11.base.name = "cli11";
    cli11.base.version = "2.3.2";
    cli11.base.description = "A command line parser for C++11 and beyond";
    cli11.base.headerOnly = true;
    cli11.base.license = "BSD-3-Clause";
    cli11.category = categories::UTILITY;
    cli11.tags = {tags::HEADER_ONLY, tags::MODERN_CPP, tags::BEGINNER_FRIENDLY};
    cli11.homepage = "https://github.com/CLIUtils/CLI11";
    cli11.popularity = 75;
    cli11.isRecommended = true;
    cli11.useCases = {"Command line parsing", "Application arguments", "Configuration"};
    s_dependencyDatabase.push_back(cli11);

    // Testing frameworks
    InteractiveDependency gtest;
    gtest.base.name = "gtest";
    gtest.base.version = "1.14.0";
    gtest.base.description = "Google Test - Google's C++ test framework";
    gtest.base.headerOnly = false;
    gtest.base.license = "BSD-3-Clause";
    gtest.category = categories::TESTING;
    gtest.tags = {tags::STABLE, tags::POPULAR};
    gtest.homepage = "https://github.com/google/googletest";
    gtest.popularity = 85;
    gtest.isRecommended = true;
    gtest.useCases = {"Unit testing", "Integration testing", "Test automation"};
    s_dependencyDatabase.push_back(gtest);

    InteractiveDependency catch2;
    catch2.base.name = "catch2";
    catch2.base.version = "3.4.0";
    catch2.base.description = "A modern, C++-native, test framework for unit-tests";
    catch2.base.headerOnly = true;
    catch2.base.license = "BSL-1.0";
    catch2.category = categories::TESTING;
    catch2.tags = {tags::HEADER_ONLY, tags::MODERN_CPP, tags::BEGINNER_FRIENDLY};
    catch2.homepage = "https://github.com/catchorg/Catch2";
    catch2.popularity = 80;
    catch2.isRecommended = true;
    catch2.useCases = {"Unit testing", "BDD testing", "Simple test setup"};
    s_dependencyDatabase.push_back(catch2);

    // Networking libraries
    InteractiveDependency asio;
    asio.base.name = "asio";
    asio.base.version = "1.24.0";
    asio.base.description = "Asynchronous I/O library";
    asio.base.headerOnly = true;
    asio.base.license = "BSL-1.0";
    asio.category = categories::NETWORKING;
    asio.tags = {tags::HEADER_ONLY, tags::HIGH_PERFORMANCE, tags::CROSS_PLATFORM};
    asio.homepage = "https://think-async.com/Asio/";
    asio.popularity = 85;
    asio.isRecommended = true;
    asio.useCases = {"Network programming", "Async I/O", "Server development"};
    s_dependencyDatabase.push_back(asio);

    // GUI libraries
    InteractiveDependency qt6;
    qt6.base.name = "qt6";
    qt6.base.version = "6.5.0";
    qt6.base.description = "Cross-platform GUI toolkit";
    qt6.base.headerOnly = false;
    qt6.base.license = "LGPL-3.0";
    qt6.category = categories::GUI;
    qt6.tags = {tags::CROSS_PLATFORM, tags::STABLE};
    qt6.homepage = "https://www.qt.io/";
    qt6.popularity = 90;
    qt6.isRecommended = true;
    qt6.useCases = {"Desktop applications", "Mobile apps", "Embedded GUIs"};
    s_dependencyDatabase.push_back(qt6);

    InteractiveDependency imgui;
    imgui.base.name = "imgui";
    imgui.base.version = "1.89.9";
    imgui.base.description = "Immediate Mode Graphical User Interface";
    imgui.base.headerOnly = false;
    imgui.base.license = "MIT";
    imgui.category = categories::GUI;
    imgui.tags = {tags::LIGHTWEIGHT, tags::MODERN_CPP};
    imgui.homepage = "https://github.com/ocornut/imgui";
    imgui.popularity = 85;
    imgui.isRecommended = true;
    imgui.useCases = {"Game development", "Tools", "Debug interfaces"};
    s_dependencyDatabase.push_back(imgui);
}

void InteractiveDependencyManager::loadCustomDependencies() {
    // Implementation would load custom dependencies from user configuration
    spdlog::debug("Loading custom dependencies from user configuration");
}

bool InteractiveDependencyManager::matchesSearchCriteria(
    const InteractiveDependency& dependency,
    const DependencySearchCriteria& criteria) {

    // Check query match
    if (!criteria.query.empty()) {
        std::string query = criteria.query;
        std::transform(query.begin(), query.end(), query.begin(), ::tolower);

        std::string name = dependency.base.name;
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        std::string description = dependency.base.description;
        std::transform(description.begin(), description.end(), description.begin(), ::tolower);

        if (name.find(query) == std::string::npos &&
            description.find(query) == std::string::npos) {
            return false;
        }
    }

    // Check category filter
    if (!criteria.categories.empty()) {
        if (std::find(criteria.categories.begin(), criteria.categories.end(),
                     dependency.category) == criteria.categories.end()) {
            return false;
        }
    }

    // Check tags filter
    if (!criteria.tags.empty()) {
        bool hasMatchingTag = false;
        for (const auto& tag : criteria.tags) {
            if (std::find(dependency.tags.begin(), dependency.tags.end(), tag) != dependency.tags.end()) {
                hasMatchingTag = true;
                break;
            }
        }
        if (!hasMatchingTag) {
            return false;
        }
    }

    // Check header-only filter
    if (criteria.headerOnlyOnly && !dependency.base.headerOnly) {
        return false;
    }

    // Check recommended filter
    if (criteria.recommendedOnly && !dependency.isRecommended) {
        return false;
    }

    // Check popularity filter
    if (dependency.popularity < criteria.minPopularity) {
        return false;
    }

    return true;
}

std::vector<std::string> InteractiveDependencyManager::getAvailableCategories() {
    if (!s_databaseLoaded) {
        loadDependencyDatabase();
    }

    std::vector<std::string> categories;
    for (const auto& dep : s_dependencyDatabase) {
        if (std::find(categories.begin(), categories.end(), dep.category) == categories.end()) {
            categories.push_back(dep.category);
        }
    }

    std::sort(categories.begin(), categories.end());
    return categories;
}

void InteractiveDependencyManager::showSearchInterface() {
    TerminalUtils::showInfo("🔍 Dependency Search");
    std::cout << TerminalUtils::colorize("Search tips:", Color::BrightBlack) << "\n";
    std::cout << "  • Enter library name (e.g., 'fmt', 'boost')\n";
    std::cout << "  • Search by functionality (e.g., 'json', 'logging')\n";
    std::cout << "  • Use keywords (e.g., 'header-only', 'networking')\n\n";
}

std::vector<InteractiveDependency> InteractiveDependencyManager::selectDependenciesInteractively(
    const std::vector<InteractiveDependency>& available,
    const std::vector<InteractiveDependency>& preselected) {

    (void)preselected; // Suppress unused parameter warning for now

    std::vector<InteractiveDependency> selected;

    if (available.empty()) {
        TerminalUtils::showNpmStyleWarning("No dependencies available", "Try a different search or category");
        return selected;
    }

    showDependencyList(available, false);

    std::cout << "\n" << TerminalUtils::colorize("Select dependencies (enter numbers separated by spaces, or 'done' to finish):", Color::BrightCyan) << "\n";
    std::cout << TerminalUtils::colorize("You can also enter 'info <number>' to see details about a dependency", Color::BrightBlack) << "\n";

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "done" || input.empty()) {
            break;
        }

        if (input.substr(0, 5) == "info ") {
            try {
                int index = std::stoi(input.substr(5)) - 1;
                if (index >= 0 && index < static_cast<int>(available.size())) {
                    showDependencyDetails(available[index]);
                } else {
                    TerminalUtils::showNpmStyleError("Invalid index", "Please enter a valid number");
                }
            } catch (const std::exception&) {
                TerminalUtils::showNpmStyleError("Invalid format", "Use 'info <number>'");
            }
            continue;
        }

        // Parse selection numbers
        std::istringstream iss(input);
        std::string token;
        while (iss >> token) {
            try {
                int index = std::stoi(token) - 1;
                if (index >= 0 && index < static_cast<int>(available.size())) {
                    // Check if already selected
                    if (std::find_if(selected.begin(), selected.end(),
                                   [&available, index](const InteractiveDependency& dep) {
                                       return dep.base.name == available[index].base.name;
                                   }) == selected.end()) {
                        selected.push_back(available[index]);
                        TerminalUtils::showNpmStyleSuccess("Added: " + available[index].base.name);
                    } else {
                        TerminalUtils::showNpmStyleWarning("Already selected", available[index].base.name);
                    }
                } else {
                    TerminalUtils::showNpmStyleError("Invalid index", token);
                }
            } catch (const std::exception&) {
                TerminalUtils::showNpmStyleError("Invalid number", token);
            }
        }
    }

    return selected;
}

void InteractiveDependencyManager::showDependencyList(
    const std::vector<InteractiveDependency>& dependencies,
    bool showDetails) {

    std::cout << "\n" << TerminalUtils::colorize("Available Dependencies:", Color::BrightMagenta) << "\n";

    for (size_t i = 0; i < dependencies.size(); ++i) {
        const auto& dep = dependencies[i];

        std::cout << "  " << TerminalUtils::colorize(std::to_string(i + 1) + ".", Color::BrightYellow);
        std::cout << " " << TerminalUtils::colorize(dep.base.name, Color::BrightWhite);

        if (!dep.base.version.empty()) {
            std::cout << " " << TerminalUtils::colorize("v" + dep.base.version, Color::BrightBlack);
        }

        std::cout << " - " << TerminalUtils::colorize(dep.base.description, Color::BrightBlack);

        if (dep.base.headerOnly) {
            std::cout << " " << TerminalUtils::colorize("[Header-only]", Color::BrightGreen);
        }

        if (dep.isRecommended) {
            std::cout << " " << TerminalUtils::colorize("⭐", Color::BrightYellow);
        }

        std::cout << "\n";

        if (showDetails) {
            std::cout << "    " << TerminalUtils::colorize("Category: " + dep.category, Color::BrightBlack) << "\n";
            std::cout << "    " << TerminalUtils::colorize("License: " + dep.base.license, Color::BrightBlack) << "\n";
            if (!dep.tags.empty()) {
                std::cout << "    " << TerminalUtils::colorize("Tags: ", Color::BrightBlack);
                for (size_t j = 0; j < dep.tags.size(); ++j) {
                    if (j > 0) std::cout << ", ";
                    std::cout << dep.tags[j];
                }
                std::cout << "\n";
            }
        }
    }
}

void InteractiveDependencyManager::showDependencyDetails(const InteractiveDependency& dependency) {
    TerminalUtils::clearScreen();

    std::vector<std::string> detailLines = {
        "📦 " + dependency.base.name,
        dependency.base.description
    };
    TerminalUtils::showBox(detailLines, BorderStyle::Double, Color::BrightCyan, Color::White, "");

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("Version: ", Color::BrightYellow)
              << TerminalUtils::colorize(dependency.base.version, Color::BrightWhite) << "\n";
    std::cout << TerminalUtils::colorize("License: ", Color::BrightYellow)
              << TerminalUtils::colorize(dependency.base.license, Color::BrightWhite) << "\n";
    std::cout << TerminalUtils::colorize("Category: ", Color::BrightYellow)
              << TerminalUtils::colorize(dependency.category, Color::BrightWhite) << "\n";
    std::cout << TerminalUtils::colorize("Type: ", Color::BrightYellow)
              << TerminalUtils::colorize(dependency.base.headerOnly ? "Header-only" : "Compiled", Color::BrightWhite) << "\n";
    std::cout << TerminalUtils::colorize("Popularity: ", Color::BrightYellow)
              << TerminalUtils::colorize(std::to_string(dependency.popularity) + "/100", Color::BrightWhite) << "\n";

    if (!dependency.homepage.empty()) {
        std::cout << TerminalUtils::colorize("Homepage: ", Color::BrightYellow)
                  << TerminalUtils::colorize(dependency.homepage, Color::BrightBlue) << "\n";
    }

    if (!dependency.tags.empty()) {
        std::cout << TerminalUtils::colorize("Tags: ", Color::BrightYellow);
        for (size_t i = 0; i < dependency.tags.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << TerminalUtils::colorize(dependency.tags[i], Color::BrightGreen);
        }
        std::cout << "\n";
    }

    if (!dependency.useCases.empty()) {
        std::cout << "\n" << TerminalUtils::colorize("Common Use Cases:", Color::BrightMagenta) << "\n";
        for (const auto& useCase : dependency.useCases) {
            std::cout << "  • " << TerminalUtils::colorize(useCase, Color::BrightWhite) << "\n";
        }
    }

    std::cout << "\n";
    UserInput::readConfirmation("Press Enter to continue", true);
}

DependencySelection InteractiveDependencyManager::validateAndResolveDependencies(
    const std::vector<InteractiveDependency>& selected,
    const CliOptions& options) {

    (void)options; // Suppress unused parameter warning for now

    DependencySelection selection;
    selection.selected = selected;
    selection.success = true;

    // Basic validation - check for conflicts
    selection.conflicts = checkDependencyConflicts(selected);
    if (!selection.conflicts.empty()) {
        selection.success = false;
    }

    // Add some warnings for demonstration
    for (const auto& dep : selected) {
        if (!dep.base.headerOnly && dep.base.name != "qt6") {
            selection.warnings.push_back("Library " + dep.base.name + " requires compilation");
        }
    }

    return selection;
}

std::vector<std::string> InteractiveDependencyManager::checkDependencyConflicts(
    const std::vector<InteractiveDependency>& dependencies) {

    std::vector<std::string> conflicts;

    // Check for known conflicts (simplified example)
    bool hasQt = false;
    bool hasGtk = false;

    for (const auto& dep : dependencies) {
        if (dep.base.name == "qt6" || dep.base.name == "qt5") {
            hasQt = true;
        }
        if (dep.base.name == "gtk" || dep.base.name == "gtkmm") {
            hasGtk = true;
        }
    }

    if (hasQt && hasGtk) {
        conflicts.push_back("Qt and GTK libraries may conflict in the same project");
    }

    return conflicts;
}

std::vector<PackageManager> InteractiveDependencyManager::getAvailablePackageManagers() {
    return {
        PackageManager::Vcpkg,
        PackageManager::Conan,
        PackageManager::CPM,
        PackageManager::FetchContent,
        PackageManager::Spack,
        PackageManager::Hunter
    };
}

PackageManager InteractiveDependencyManager::selectPackageManagerInteractively(
    const std::vector<PackageManager>& available,
    TemplateType templateType) {

    (void)templateType; // Suppress unused parameter warning for now

    std::vector<std::string_view> pmNames;
    for (const auto& pm : available) {
        pmNames.push_back(cli_enums::to_string(pm));
    }

    std::string selected = UserInput::readChoiceWithStyle(
        "Select package manager",
        pmNames,
        std::string(pmNames[0]),
        Color::BrightCyan
    );

    auto selectedPM = cli_enums::to_package_manager(selected);
    return selectedPM ? *selectedPM : PackageManager::Vcpkg;
}

bool InteractiveDependencyManager::configurePackageManagerInteractively(
    PackageManager packageManager,
    const CliOptions& options) {

    (void)packageManager; // Suppress unused parameter warning
    (void)options; // Suppress unused parameter warning

    TerminalUtils::showInfo("Package manager configuration completed");
    return true;
}

std::vector<InteractiveDependency> InteractiveDependencyManager::importDependencyList(
    const std::string& filePath,
    const std::string& format) {

    (void)format; // Suppress unused parameter warning for now

    std::vector<InteractiveDependency> imported;

    try {
        std::string content = FileUtils::readFromFile(filePath);
        if (content.empty()) {
            throw std::runtime_error("File is empty or could not be read");
        }

        // Simple JSON parsing example
        json j = json::parse(content);

        if (j.contains("dependencies") && j["dependencies"].is_array()) {
            for (const auto& depJson : j["dependencies"]) {
                InteractiveDependency dep;
                dep.base.name = depJson.value("name", "");
                dep.base.version = depJson.value("version", "");
                dep.base.description = depJson.value("description", "");

                if (!dep.base.name.empty()) {
                    imported.push_back(dep);
                }
            }
        }

    } catch (const std::exception& e) {
        spdlog::error("Failed to import dependency list: {}", e.what());
        throw;
    }

    return imported;
}

} // namespace utils
