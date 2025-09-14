#include "user_input.h"

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

#include "../../utils/core/string_utils.h"
#include "../../utils/ui/terminal_utils.h"

using namespace utils;

// Input reading with validation
std::string UserInput::read(std::string_view prompt, std::optional<std::string_view> defaultValue,
                            std::optional<ValidatorFunction> validator) {
    std::string input;

    while (true) {
        // Display prompt and default value
        if (defaultValue.has_value()) {
            fmt::print("{} [{}]: ", prompt, *defaultValue);
        } else {
            fmt::print("{}: ", prompt);
        }

        // Get user input
        std::getline(std::cin, input);

        // Use default value if input is empty and default exists
        if (input.empty() && defaultValue.has_value()) {
            return std::string(*defaultValue);
        }

        // Validate input
        if (validator.has_value()) {
            auto result = (*validator)(input);

            if (std::holds_alternative<bool>(result)) {
                if (std::get<bool>(result)) {
                    break;  // Validation passed
                }
            } else {
                // Display validation error message
                std::cout << TerminalUtils::colorize(std::get<std::string>(result),
                                                     utils::Color::BrightRed)
                          << std::endl;
            }
        } else {
            break;  // No validation needed
        }
    }

    return input;
}

// Create common validators
UserInput::ValidatorFunction UserInput::notEmptyValidator() {
    return [](const std::string& input) -> ValidatorResult {
        return !input.empty() || "Input cannot be empty";
    };
}

UserInput::ValidatorFunction UserInput::numericValidator() {
    return [](const std::string& input) -> ValidatorResult {
        if (input.empty())
            return "Input cannot be empty";

        for (char c : input) {
            if (!std::isdigit(c)) {
                return "Input must be numeric";
            }
        }
        return true;
    };
}

UserInput::ValidatorFunction UserInput::rangeValidator(int min, int max) {
    return [min, max](const std::string& input) -> ValidatorResult {
        try {
            int value = std::stoi(input);
            if (value < min || value > max) {
                return fmt::format("Value must be between {} and {}", min, max);
            }
            return true;
        } catch (...) {
            return "Invalid number format";
        }
    };
}

UserInput::ValidatorFunction UserInput::oneOfValidator(
        const std::vector<std::string_view>& validValues) {
    return [validValues](const std::string& input) -> ValidatorResult {
        for (const auto& valid : validValues) {
            if (StringUtils::toLower(input) == StringUtils::toLower(std::string(valid))) {
                return true;
            }
        }

        std::string validOptions;
        for (size_t i = 0; i < validValues.size(); ++i) {
            if (i > 0)
                validOptions += ", ";
            validOptions += validValues[i];
        }

        return fmt::format("Input must be one of: {}", validOptions);
    };
}

// Read user confirmation
bool UserInput::readConfirmation(std::string_view prompt, bool defaultValue) {
    std::string defaultStr = defaultValue ? "Y/n" : "y/N";
    std::string input = read(prompt, defaultStr);

    if (input.empty()) {
        return defaultValue;
    }

    std::string lowerInput = StringUtils::toLower(input);
    return lowerInput == "y" || lowerInput == "yes";
}

// Read number with range validation
int UserInput::readNumber(std::string_view prompt, int min, int max,
                          std::optional<int> defaultValue) {
    std::optional<std::string_view> defaultStr;
    if (defaultValue.has_value()) {
        static std::string defaultString = std::to_string(*defaultValue);
        defaultStr = defaultString;
    }

    auto validator = rangeValidator(min, max);
    std::string input = read(prompt, defaultStr, validator);

    return std::stoi(input);
}

// Read user choice from options
std::string UserInput::readChoice(std::string_view prompt,
                                  const std::vector<std::string_view>& choices,
                                  std::optional<std::string_view> defaultValue) {
    fmt::print("{}\n", prompt);
    printOptions(choices, defaultValue);

    while (true) {
        std::string input = read("Please select", defaultValue);

        // Check if input is a number
        if (std::all_of(input.begin(), input.end(), [](char c) { return std::isdigit(c); })) {
            int index = std::stoi(input) - 1;
            if (index >= 0 && index < static_cast<int>(choices.size())) {
                return std::string(choices[index]);
            }
        }
        // Check if input matches an option
        else {
            for (const auto& choice : choices) {
                if (StringUtils::toLower(input) == StringUtils::toLower(std::string(choice))) {
                    return std::string(choice);
                }
            }
        }

        std::cout << TerminalUtils::colorize("Invalid selection, please try again.",
                                             utils::Color::BrightRed)
                  << std::endl;
    }
}

// Enhanced input with highlighting
std::string UserInput::readWithHighlight(std::string_view prompt,
                                         std::optional<std::string_view> defaultValue,
                                         utils::Color promptColor) {
    std::string input;

    // Display highlighted prompt
    if (defaultValue.has_value()) {
        std::cout << TerminalUtils::colorize(std::string(prompt), promptColor) << " ["
                  << TerminalUtils::colorize(std::string(*defaultValue), utils::Color::BrightGreen)
                  << "]: ";
    } else {
        std::cout << TerminalUtils::colorize(std::string(prompt), promptColor) << ": ";
    }

    std::getline(std::cin, input);

    if (input.empty() && defaultValue.has_value()) {
        return std::string(*defaultValue);
    }

    return input;
}

// Choice reading with styling
std::string UserInput::readChoiceWithStyle(std::string_view prompt,
                                           const std::vector<std::string_view>& choices,
                                           std::optional<std::string_view> defaultValue,
                                           utils::Color highlightColor) {
    std::cout << TerminalUtils::colorize(std::string(prompt), utils::Color::BrightCyan) << "\n";
    printStyledOptions(choices, defaultValue, highlightColor);

    while (true) {
        std::string input =
                readWithHighlight("Please select", defaultValue, utils::Color::BrightYellow);

        // Check if input is a number
        if (std::all_of(input.begin(), input.end(), [](char c) { return std::isdigit(c); })) {
            int index = std::stoi(input) - 1;
            if (index >= 0 && index < static_cast<int>(choices.size())) {
                return std::string(choices[index]);
            }
        }
        // Check if input matches an option
        else {
            for (const auto& choice : choices) {
                if (StringUtils::toLower(input) == StringUtils::toLower(std::string(choice))) {
                    return std::string(choice);
                }
            }
        }

        std::cout << TerminalUtils::colorize("Invalid selection, please try again.",
                                             utils::Color::BrightRed)
                  << "\n";
    }
}

// Multi-choice selection
std::vector<std::string> UserInput::readMultiChoice(
        std::string_view prompt, const std::vector<std::string_view>& choices,
        const std::vector<std::string_view>& defaultValues) {
    std::vector<std::string> selectedOptions;

    // Convert default values to strings
    for (const auto& val : defaultValues) {
        selectedOptions.push_back(std::string(val));
    }

    fmt::print("{}\n", prompt);
    printMultiOptions(choices, selectedOptions);

    fmt::print("Enter numbers to toggle selection (comma or space separated),\n");
    fmt::print("or 'all' to select all, 'none' to clear, 'done' when finished: ");

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        input = StringUtils::toLower(input);

        if (input == "done") {
            break;
        } else if (input == "all") {
            selectedOptions.clear();
            for (const auto& choice : choices) {
                selectedOptions.push_back(std::string(choice));
            }
            printMultiOptions(choices, selectedOptions);
        } else if (input == "none") {
            selectedOptions.clear();
            printMultiOptions(choices, selectedOptions);
        } else {
            // Parse input, supporting comma and space separation
            std::vector<std::string> selections;
            std::string token;
            std::istringstream tokenStream(input);

            while (std::getline(tokenStream, token, ',')) {
                token = StringUtils::trim(token);
                if (!token.empty()) {
                    selections.push_back(token);
                }
            }

            // Process each selection
            for (const auto& selection : selections) {
                // Try as number first
                if (std::all_of(selection.begin(), selection.end(),
                                [](char c) { return std::isdigit(c); })) {
                    int index = std::stoi(selection) - 1;
                    if (index >= 0 && index < static_cast<int>(choices.size())) {
                        std::string choice = std::string(choices[index]);
                        // Toggle selection state
                        auto it = std::find(selectedOptions.begin(), selectedOptions.end(), choice);
                        if (it != selectedOptions.end()) {
                            selectedOptions.erase(it);
                        } else {
                            selectedOptions.push_back(choice);
                        }
                    }
                }
                // Try as option name
                else {
                    for (const auto& choice : choices) {
                        if (StringUtils::toLower(selection) ==
                            StringUtils::toLower(std::string(choice))) {
                            std::string choiceStr = std::string(choice);
                            // Toggle selection state
                            auto it = std::find(selectedOptions.begin(), selectedOptions.end(),
                                                choiceStr);
                            if (it != selectedOptions.end()) {
                                selectedOptions.erase(it);
                            } else {
                                selectedOptions.push_back(choiceStr);
                            }
                            break;
                        }
                    }
                }
            }

            printMultiOptions(choices, selectedOptions);
        }

        fmt::print("Continue selecting or type 'done' to finish: ");
    }

    return selectedOptions;
}

// Print options helper
void UserInput::printOptions(const std::vector<std::string_view>& options,
                             std::optional<std::string_view> defaultOption) {
    for (size_t i = 0; i < options.size(); i++) {
        std::string marker;
        if (defaultOption.has_value() && options[i] == *defaultOption) {
            marker = " (default)";
            fmt::print("  {}. **{}**{}\n", i + 1, options[i], marker);
        } else {
            fmt::print("  {}. {}\n", i + 1, options[i]);
        }
    }
}

// Print multi-choice options helper
void UserInput::printMultiOptions(const std::vector<std::string_view>& options,
                                  const std::vector<std::string>& selectedOptions) {
    for (size_t i = 0; i < options.size(); i++) {
        bool isSelected = std::find(selectedOptions.begin(), selectedOptions.end(),
                                    std::string(options[i])) != selectedOptions.end();

        std::string marker = isSelected ? " [x]" : " [ ]";

        if (isSelected) {
            fmt::print("  {}. **{}**{}\n", i + 1, options[i], marker);
        } else {
            fmt::print("  {}. {}{}\n", i + 1, options[i], marker);
        }
    }
}

// Print styled options helper
void UserInput::printStyledOptions(const std::vector<std::string_view>& options,
                                   std::optional<std::string_view> defaultOption,
                                   utils::Color highlightColor) {
    for (size_t i = 0; i < options.size(); i++) {
        std::string numberStr = std::to_string(i + 1) + ".";
        std::cout << "  " << TerminalUtils::colorize(numberStr, utils::Color::BrightBlack) << " ";

        if (defaultOption.has_value() && options[i] == *defaultOption) {
            std::cout << TerminalUtils::colorize(std::string(options[i]), highlightColor)
                      << TerminalUtils::colorize(" (default)", utils::Color::BrightBlack) << "\n";
        } else {
            std::cout << TerminalUtils::colorize(std::string(options[i]), utils::Color::White)
                      << "\n";
        }
    }
}
