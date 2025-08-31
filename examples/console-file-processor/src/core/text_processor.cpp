/**
 * @file text_processor.cpp
 * @brief Implementation of text processing utilities
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include "text_processor.h"
#include <sstream>
#include <algorithm>

namespace core {

std::size_t TextProcessor::count_words(const std::string& text) {
    std::istringstream iss(text);
    std::string word;
    std::size_t count = 0;
    
    while (iss >> word) {
        ++count;
    }
    
    return count;
}

std::size_t TextProcessor::count_lines(const std::string& text) {
    if (text.empty()) {
        return 0;
    }
    
    return std::count(text.begin(), text.end(), '\n') + 1;
}

std::size_t TextProcessor::count_characters(const std::string& text) {
    return text.length();
}

std::vector<std::pair<std::size_t, std::string>> TextProcessor::search_pattern(
    const std::string& text, 
    const std::string& pattern, 
    bool case_sensitive) {
    
    std::vector<std::pair<std::size_t, std::string>> matches;
    
    try {
        std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;
        if (!case_sensitive) {
            flags |= std::regex_constants::icase;
        }
        
        std::regex regex_pattern(pattern, flags);
        std::sregex_iterator iter(text.begin(), text.end(), regex_pattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            const std::smatch& match = *iter;
            matches.emplace_back(match.position(), match.str());
        }
    } catch (const std::regex_error&) {
        // If regex fails, try simple string search
        std::string search_text = text;
        std::string search_pattern = pattern;
        
        if (!case_sensitive) {
            std::transform(search_text.begin(), search_text.end(), search_text.begin(), ::tolower);
            std::transform(search_pattern.begin(), search_pattern.end(), search_pattern.begin(), ::tolower);
        }
        
        std::size_t pos = 0;
        while ((pos = search_text.find(search_pattern, pos)) != std::string::npos) {
            matches.emplace_back(pos, text.substr(pos, search_pattern.length()));
            pos += search_pattern.length();
        }
    }
    
    return matches;
}

std::string TextProcessor::replace_pattern(
    const std::string& text,
    const std::string& pattern,
    const std::string& replacement) {
    
    try {
        std::regex regex_pattern(pattern);
        return std::regex_replace(text, regex_pattern, replacement);
    } catch (const std::regex_error&) {
        // Fallback to simple string replacement
        std::string result = text;
        std::size_t pos = 0;
        
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.replace(pos, pattern.length(), replacement);
            pos += replacement.length();
        }
        
        return result;
    }
}

} // namespace core
