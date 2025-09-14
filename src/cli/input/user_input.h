#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "../../utils/ui/terminal_utils.h"
#include "../types/cli_enums.h"


// User input handling class for CLI interactions
class UserInput {
   public:
    // Type-safe input validation variant type
    using ValidatorResult = std::variant<bool, std::string>;
    using ValidatorFunction = std::function<ValidatorResult(const std::string&)>;

    // Input reading with validation
    static std::string read(std::string_view prompt,
                            std::optional<std::string_view> defaultValue = std::nullopt,
                            std::optional<ValidatorFunction> validator = std::nullopt);

    // Create common validators
    static ValidatorFunction notEmptyValidator();
    static ValidatorFunction numericValidator();
    static ValidatorFunction rangeValidator(int min, int max);
    static ValidatorFunction oneOfValidator(const std::vector<std::string_view>& validValues);

    // Specialized input methods
    static bool readConfirmation(std::string_view prompt, bool defaultValue = true);
    static int readNumber(std::string_view prompt, int min, int max,
                          std::optional<int> defaultValue = std::nullopt);
    static std::string readChoice(std::string_view prompt,
                                  const std::vector<std::string_view>& choices,
                                  std::optional<std::string_view> defaultValue = std::nullopt);
    static std::vector<std::string> readMultiChoice(
            std::string_view prompt, const std::vector<std::string_view>& choices,
            const std::vector<std::string_view>& defaultValues = {});

    // Enhanced interactive input functions
    static std::string readWithHighlight(
            std::string_view prompt, std::optional<std::string_view> defaultValue = std::nullopt,
            utils::Color promptColor = utils::Color::BrightCyan);

    static std::string readChoiceWithStyle(
            std::string_view prompt, const std::vector<std::string_view>& choices,
            std::optional<std::string_view> defaultValue = std::nullopt,
            utils::Color highlightColor = utils::Color::BrightGreen);

   private:
    // Output formatting helper methods
    static void printOptions(const std::vector<std::string_view>& options,
                             std::optional<std::string_view> defaultOption = std::nullopt);
    static void printMultiOptions(const std::vector<std::string_view>& options,
                                  const std::vector<std::string>& selectedOptions);

    // Styled option display using terminal utilities
    static void printStyledOptions(const std::vector<std::string_view>& options,
                                   std::optional<std::string_view> defaultOption = std::nullopt,
                                   utils::Color highlightColor = utils::Color::BrightGreen);
};
