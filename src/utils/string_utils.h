#pragma once
#include <string>
#include <vector>
#include <regex>
#include <optional>
#include <unordered_map>
#include <functional>
#include <sstream>

namespace utils {

// String validation result
struct StringValidationResult {
    bool valid = false;
    std::string reason;
    
    operator bool() const { return valid; }
};

// String format options
struct FormatOptions {
    char paddingChar = ' ';
    size_t width = 0;
    bool leftAlign = true;
    bool trimSpaces = true;
};

class StringUtils {
public:
    // Enhanced case conversion
    static std::string toLower(const std::string &str);
    static std::string toUpper(const std::string &str);
    static std::string toTitleCase(const std::string &str);
    static std::string toCamelCase(const std::string &str);
    static std::string toSnakeCase(const std::string &str);
    static std::string toKebabCase(const std::string &str);
    static std::string toPascalCase(const std::string &str);
    
    // Enhanced string manipulation
    static std::string replace(const std::string &str, const std::string &from,
                             const std::string &to);
    static std::string replaceAll(const std::string &str, const std::string &from,
                                const std::string &to);
    static std::string replaceRegex(const std::string &str, const std::regex &pattern,
                                  const std::string &replacement);
    static std::vector<std::string> split(const std::string &str, char delimiter);
    static std::vector<std::string> split(const std::string &str, const std::string &delimiter);
    static std::vector<std::string> splitRegex(const std::string &str, const std::regex &pattern);
    static std::string join(const std::vector<std::string> &strings, const std::string &delimiter);
    static std::string reverse(const std::string &str);
    
    // Enhanced whitespace handling
    static std::string trim(const std::string &str);
    static std::string trimLeft(const std::string &str);
    static std::string trimRight(const std::string &str);
    static std::string trimChars(const std::string &str, const std::string &chars);
    static std::string removeWhitespace(const std::string &str);
    static std::string normalizeWhitespace(const std::string &str);
    static std::string collapseSpaces(const std::string &str);
    
    // String queries and testing
    static bool startsWith(const std::string &str, const std::string &prefix);
    static bool endsWith(const std::string &str, const std::string &suffix);
    static bool startsWithIgnoreCase(const std::string &str, const std::string &prefix);
    static bool endsWithIgnoreCase(const std::string &str, const std::string &suffix);
    static bool contains(const std::string &str, const std::string &substring);
    static bool containsIgnoreCase(const std::string &str, const std::string &substring);
    static bool isEmpty(const std::string &str);
    static bool isWhitespace(const std::string &str);
    static bool isNumeric(const std::string &str);
    static bool isInteger(const std::string &str);
    static bool isFloat(const std::string &str);
    static bool isAlphabetic(const std::string &str);
    static bool isAlphanumeric(const std::string &str);
    static bool isHexadecimal(const std::string &str);
    
    // Advanced validation
    static StringValidationResult isValidIdentifier(const std::string &str);
    static StringValidationResult isValidProjectName(const std::string &str);
    static StringValidationResult isValidClassName(const std::string &str);
    static StringValidationResult isValidEmail(const std::string &str);
    static StringValidationResult isValidUrl(const std::string &str);
    static StringValidationResult isValidVersion(const std::string &str);
    static StringValidationResult isValidPath(const std::string &str);
    static StringValidationResult isValidFileName(const std::string &str);
    
    // String formatting and padding
    static std::string format(const std::string &formatStr, const std::vector<std::string> &args);
    static std::string padLeft(const std::string &str, size_t width, char fillChar = ' ');
    static std::string padRight(const std::string &str, size_t width, char fillChar = ' ');
    static std::string padCenter(const std::string &str, size_t width, char fillChar = ' ');
    static std::string repeat(const std::string &str, size_t count);
    static std::string truncate(const std::string &str, size_t maxLength, const std::string &suffix = "...");
    
    // String comparison
    static bool equalsIgnoreCase(const std::string &str1, const std::string &str2);
    static int compareIgnoreCase(const std::string &str1, const std::string &str2);
    static bool isSubsequence(const std::string &str, const std::string &subsequence);
    static size_t levenshteinDistance(const std::string &str1, const std::string &str2);
    static double similarityRatio(const std::string &str1, const std::string &str2);
    
    // Encoding and escaping
    static std::string escapeJson(const std::string &str);
    static std::string unescapeJson(const std::string &str);
    static std::string escapeXml(const std::string &str);
    static std::string unescapeXml(const std::string &str);
    static std::string escapeShell(const std::string &str);
    static std::string escapeRegex(const std::string &str);
    static std::string urlEncode(const std::string &str);
    static std::string urlDecode(const std::string &str);
    
    // String analysis
    static size_t countOccurrences(const std::string &str, const std::string &substring);
    static size_t countOccurrences(const std::string &str, char ch);
    static std::vector<size_t> findAllOccurrences(const std::string &str, const std::string &substring);
    static std::unordered_map<char, size_t> getCharacterFrequency(const std::string &str);
    static std::string getLongestCommonPrefix(const std::vector<std::string> &strings);
    static std::string getLongestCommonSuffix(const std::vector<std::string> &strings);
    
    // Text processing
    static std::vector<std::string> splitLines(const std::string &str);
    static std::string joinLines(const std::vector<std::string> &lines);
    static std::string wordWrap(const std::string &str, size_t width);
    static std::vector<std::string> extractWords(const std::string &str);
    static size_t countWords(const std::string &str);
    static size_t countLines(const std::string &str);
    
    // Utility functions
    static std::string generateRandomString(size_t length, const std::string &charset = "");
    static std::string generateUuid();
    static std::string getCurrentTimestamp(const std::string &format = "%Y-%m-%d %H:%M:%S");
    static std::string formatBytes(std::uintmax_t bytes);
    static std::string formatDuration(std::chrono::milliseconds duration);
    
    // Template and placeholder replacement
    static std::string replacePlaceholders(const std::string &templateStr, 
                                          const std::unordered_map<std::string, std::string> &replacements);
    static std::vector<std::string> extractPlaceholders(const std::string &templateStr, 
                                                       const std::string &prefix = "{", 
                                                       const std::string &suffix = "}");
    
    // Hash and checksum utilities
    static std::string calculateMD5(const std::string &str);
    static std::string calculateSHA256(const std::string &str);
    static uint32_t calculateCRC32(const std::string &str);
};
} // namespace utils
