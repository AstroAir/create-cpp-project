/**
 * @file argument_parser.h
 * @brief Command-line argument parsing for the file processor
 * 
 * This file provides a wrapper around a command-line argument parsing library
 * (like argparse or CLI11) with application-specific functionality.
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <stdexcept>

namespace cli {

/**
 * @brief Exception thrown for argument parsing errors
 */
class ArgumentError : public std::runtime_error {
public:
    explicit ArgumentError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Argument definition structure
 */
struct ArgumentDef {
    std::string name;
    std::string help;
    std::any default_value;
    std::any implicit_value;
    std::vector<std::string> choices;
    bool required = false;
    int nargs = 1; // -1 for unlimited, 0 for flag, 1+ for specific count
    std::string metavar;
    std::function<std::any(const std::string&)> converter;
};

/**
 * @brief Simple command-line argument parser
 * 
 * This class provides a simplified interface for parsing command-line arguments
 * with support for flags, options, positional arguments, and validation.
 */
class ArgumentParser {
public:
    /**
     * @brief Constructor
     * @param program_name Name of the program
     * @param description Program description
     */
    ArgumentParser(const std::string& program_name, const std::string& description = "");
    
    /**
     * @brief Add a command-line argument
     * @param name Argument name (e.g., "--verbose" or "files")
     * @return Reference to ArgumentDef for chaining
     */
    ArgumentDef& add_argument(const std::string& name);
    
    /**
     * @brief Parse command-line arguments
     * @param argc Number of arguments
     * @param argv Array of argument strings
     */
    void parse_args(int argc, char* argv[]);
    
    /**
     * @brief Get parsed argument value
     * @tparam T Type to convert value to
     * @param name Argument name
     * @return Parsed value
     */
    template<typename T>
    T get(const std::string& name) const {
        auto it = parsed_values_.find(normalize_name(name));
        if (it == parsed_values_.end()) {
            auto def_it = arguments_.find(normalize_name(name));
            if (def_it != arguments_.end() && def_it->second.default_value.has_value()) {
                return std::any_cast<T>(def_it->second.default_value);
            }
            throw ArgumentError("Argument not found: " + name);
        }
        
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast& e) {
            throw ArgumentError("Type conversion error for argument: " + name);
        }
    }
    
    /**
     * @brief Check if argument was provided
     * @param name Argument name
     * @return True if argument was provided
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief Print help message
     */
    void print_help() const;
    
    /**
     * @brief Print usage message
     */
    void print_usage() const;

private:
    std::string program_name_;
    std::string description_;
    std::map<std::string, ArgumentDef> arguments_;
    std::map<std::string, std::any> parsed_values_;
    std::vector<std::string> positional_args_;
    
    /**
     * @brief Normalize argument name (remove leading dashes)
     * @param name Original name
     * @return Normalized name
     */
    std::string normalize_name(const std::string& name) const;
    
    /**
     * @brief Check if string is an option (starts with -)
     * @param arg Argument string
     * @return True if it's an option
     */
    bool is_option(const std::string& arg) const;
    
    /**
     * @brief Parse a single option
     * @param args Argument vector
     * @param index Current index (will be modified)
     */
    void parse_option(const std::vector<std::string>& args, std::size_t& index);
    
    /**
     * @brief Parse positional arguments
     * @param args Remaining positional arguments
     */
    void parse_positional(const std::vector<std::string>& args);
    
    /**
     * @brief Validate parsed arguments
     */
    void validate_arguments();
    
    /**
     * @brief Convert string to appropriate type
     * @param value String value
     * @param def Argument definition
     * @return Converted value
     */
    std::any convert_value(const std::string& value, const ArgumentDef& def);
    
    /**
     * @brief Default string converter
     * @param value String value
     * @return String value (no conversion)
     */
    static std::any string_converter(const std::string& value);
    
    /**
     * @brief Integer converter
     * @param value String value
     * @return Integer value
     */
    static std::any int_converter(const std::string& value);
    
    /**
     * @brief Boolean converter
     * @param value String value
     * @return Boolean value
     */
    static std::any bool_converter(const std::string& value);
    
    /**
     * @brief Vector<string> converter for multiple values
     * @param value String value
     * @return Vector of strings
     */
    static std::any vector_converter(const std::string& value);
};

/**
 * @brief Helper class for fluent argument definition
 */
class ArgumentBuilder {
public:
    explicit ArgumentBuilder(ArgumentDef& def) : def_(def) {}
    
    ArgumentBuilder& help(const std::string& help_text) {
        def_.help = help_text;
        return *this;
    }
    
    template<typename T>
    ArgumentBuilder& default_value(const T& value) {
        def_.default_value = value;
        return *this;
    }
    
    template<typename T>
    ArgumentBuilder& implicit_value(const T& value) {
        def_.implicit_value = value;
        return *this;
    }
    
    ArgumentBuilder& required(bool is_required = true) {
        def_.required = is_required;
        return *this;
    }
    
    ArgumentBuilder& nargs(int count) {
        def_.nargs = count;
        return *this;
    }
    
    ArgumentBuilder& choices(std::initializer_list<std::string> choice_list) {
        def_.choices = choice_list;
        return *this;
    }
    
    ArgumentBuilder& metavar(const std::string& meta) {
        def_.metavar = meta;
        return *this;
    }
    
    template<typename T>
    ArgumentBuilder& scan() {
        if constexpr (std::is_same_v<T, int>) {
            def_.converter = ArgumentParser::int_converter;
        } else if constexpr (std::is_same_v<T, bool>) {
            def_.converter = ArgumentParser::bool_converter;
        } else {
            def_.converter = ArgumentParser::string_converter;
        }
        return *this;
    }

private:
    ArgumentDef& def_;
};

} // namespace cli

// Convenience macros for argument scanning
#define SCAN_INT(parser, name) parser.get<int>(name)
#define SCAN_BOOL(parser, name) parser.get<bool>(name)
#define SCAN_STRING(parser, name) parser.get<std::string>(name)
#define SCAN_VECTOR(parser, name) parser.get<std::vector<std::string>>(name)
