/**
 * @file text_processor.h
 * @brief Text processing utilities
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <regex>

namespace core {

/**
 * @brief Text processing utilities
 */
class TextProcessor {
public:
    /**
     * @brief Count words in text
     * @param text Input text
     * @return Number of words
     */
    static std::size_t count_words(const std::string& text);
    
    /**
     * @brief Count lines in text
     * @param text Input text
     * @return Number of lines
     */
    static std::size_t count_lines(const std::string& text);
    
    /**
     * @brief Count characters in text
     * @param text Input text
     * @return Number of characters
     */
    static std::size_t count_characters(const std::string& text);
    
    /**
     * @brief Search for pattern in text
     * @param text Input text
     * @param pattern Regex pattern
     * @param case_sensitive Case sensitivity flag
     * @return Vector of match positions
     */
    static std::vector<std::pair<std::size_t, std::string>> search_pattern(
        const std::string& text, 
        const std::string& pattern, 
        bool case_sensitive = true);
    
    /**
     * @brief Replace text pattern
     * @param text Input text
     * @param pattern Pattern to replace
     * @param replacement Replacement text
     * @return Modified text
     */
    static std::string replace_pattern(
        const std::string& text,
        const std::string& pattern,
        const std::string& replacement);
};

} // namespace core
