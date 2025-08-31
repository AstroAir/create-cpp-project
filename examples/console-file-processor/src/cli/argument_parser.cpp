/**
 * @file argument_parser.cpp
 * @brief Implementation of the ArgumentParser class
 * 
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#include "argument_parser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace cli {

ArgumentParser::ArgumentParser(const std::string& program_name, const std::string& description)
    : program_name_(program_name), description_(description) {
}

ArgumentDef& ArgumentParser::add_argument(const std::string& name) {
    std::string normalized = normalize_name(name);
    arguments_[normalized] = ArgumentDef{};
    arguments_[normalized].name = name;
    return arguments_[normalized];
}

void ArgumentParser::parse_args(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    std::vector<std::string> positional;
    
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (is_option(args[i])) {
            parse_option(args, i);
        } else {
            positional.push_back(args[i]);
        }
    }
    
    parse_positional(positional);
    validate_arguments();
}

bool ArgumentParser::has(const std::string& name) const {
    return parsed_values_.find(normalize_name(name)) != parsed_values_.end();
}

void ArgumentParser::print_help() const {
    std::cout << "Usage: " << program_name_ << " [OPTIONS]";
    
    // Show positional arguments
    for (const auto& [name, def] : arguments_) {
        if (name.empty() || name[0] != '-') {
            std::cout << " " << name;
        }
    }
    std::cout << "\n\n";
    
    if (!description_.empty()) {
        std::cout << description_ << "\n\n";
    }
    
    std::cout << "Options:\n";
    for (const auto& [name, def] : arguments_) {
        if (!name.empty() && name[0] == '-') {
            std::cout << "  " << def.name;
            if (!def.metavar.empty()) {
                std::cout << " " << def.metavar;
            }
            std::cout << "\n";
            if (!def.help.empty()) {
                std::cout << "      " << def.help << "\n";
            }
        }
    }
}

void ArgumentParser::print_usage() const {
    std::cout << "Usage: " << program_name_ << " [OPTIONS]";
    for (const auto& [name, def] : arguments_) {
        if (name.empty() || name[0] != '-') {
            std::cout << " " << name;
        }
    }
    std::cout << "\n";
}

std::string ArgumentParser::normalize_name(const std::string& name) const {
    std::string normalized = name;
    while (!normalized.empty() && normalized[0] == '-') {
        normalized = normalized.substr(1);
    }
    return normalized;
}

bool ArgumentParser::is_option(const std::string& arg) const {
    return !arg.empty() && arg[0] == '-';
}

void ArgumentParser::parse_option(const std::vector<std::string>& args, std::size_t& index) {
    std::string option = args[index];
    std::string normalized = normalize_name(option);
    
    auto it = arguments_.find(normalized);
    if (it == arguments_.end()) {
        throw ArgumentError("Unknown option: " + option);
    }
    
    const auto& def = it->second;
    
    if (def.nargs == 0) {
        // Flag option
        if (def.implicit_value.has_value()) {
            parsed_values_[normalized] = def.implicit_value;
        } else {
            parsed_values_[normalized] = true;
        }
    } else {
        // Option with value
        if (index + 1 >= args.size()) {
            throw ArgumentError("Option " + option + " requires a value");
        }
        
        ++index;
        std::string value = args[index];
        parsed_values_[normalized] = convert_value(value, def);
    }
}

void ArgumentParser::parse_positional(const std::vector<std::string>& args) {
    // Find positional arguments
    std::vector<std::string> positional_names;
    for (const auto& [name, def] : arguments_) {
        if (name.empty() || name[0] != '-') {
            positional_names.push_back(name);
        }
    }
    
    if (positional_names.size() == 1 && !args.empty()) {
        // Single positional argument that takes all remaining args
        parsed_values_[positional_names[0]] = args;
    }
}

void ArgumentParser::validate_arguments() {
    for (const auto& [name, def] : arguments_) {
        if (def.required && parsed_values_.find(name) == parsed_values_.end()) {
            throw ArgumentError("Required argument missing: " + def.name);
        }
    }
}

std::any ArgumentParser::convert_value(const std::string& value, const ArgumentDef& def) {
    if (def.converter) {
        return def.converter(value);
    }
    return value;
}

std::any ArgumentParser::string_converter(const std::string& value) {
    return value;
}

std::any ArgumentParser::int_converter(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        throw ArgumentError("Invalid integer value: " + value);
    }
}

std::any ArgumentParser::bool_converter(const std::string& value) {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "true" || lower == "1" || lower == "yes") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no") {
        return false;
    } else {
        throw ArgumentError("Invalid boolean value: " + value);
    }
}

std::any ArgumentParser::vector_converter(const std::string& value) {
    std::vector<std::string> result;
    std::istringstream iss(value);
    std::string item;
    
    while (std::getline(iss, item, ',')) {
        result.push_back(item);
    }
    
    return result;
}

} // namespace cli
