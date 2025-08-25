#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace nlohmann {

class json {
public:
    // Basic types
    enum value_t {
        null,
        object_type,
        array_type,
        string_type,
        boolean_type,
        number_integer,
        number_unsigned,
        number_float
    };

private:
    value_t m_type = null;
    std::map<std::string, json> m_object;
    std::vector<json> m_array;
    std::string m_string;
    bool m_boolean = false;
    int64_t m_integer = 0;
    double m_float = 0.0;

public:
    // Constructors
    json() = default;
    json(const std::string& s) : m_type(string_type), m_string(s) {}
    json(const char* s) : m_type(string_type), m_string(s) {}
    json(bool b) : m_type(boolean_type), m_boolean(b) {}
    json(int i) : m_type(number_integer), m_integer(i) {}
    json(int64_t i) : m_type(number_integer), m_integer(i) {}
    json(double d) : m_type(number_float), m_float(d) {}

    // Static parse function
    static json parse(const std::string& s) {
        (void)s; // Suppress unused parameter warning
        json j;
        // Very basic parsing - just return empty object for now
        j.m_type = object_type;
        return j;
    }

    // Object access
    json& operator[](const std::string& key) {
        if (m_type == null) m_type = object_type;
        return m_object[key];
    }

    const json& operator[](const std::string& key) const {
        static json null_json;
        auto it = m_object.find(key);
        return (it != m_object.end()) ? it->second : null_json;
    }

    // Array access
    json& operator[](size_t index) {
        if (m_type == null) m_type = array_type;
        if (index >= m_array.size()) m_array.resize(index + 1);
        return m_array[index];
    }

    // Type checks
    bool is_null() const { return m_type == null; }
    bool is_object() const { return m_type == object_type; }
    bool is_array() const { return m_type == array_type; }
    bool is_string() const { return m_type == string_type; }
    bool is_boolean() const { return m_type == boolean_type; }
    bool is_number() const { return m_type == number_integer || m_type == number_unsigned || m_type == number_float; }

    // Value access
    template<typename T>
    T get() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return m_string;
        } else if constexpr (std::is_same_v<T, bool>) {
            return m_boolean;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<int>(m_integer);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return m_integer;
        } else if constexpr (std::is_same_v<T, double>) {
            return m_float;
        }
        return T{};
    }

    // Value with default
    template<typename T>
    T value(const std::string& key, const T& default_value) const {
        auto it = m_object.find(key);
        if (it != m_object.end()) {
            return it->second.get<T>();
        }
        return default_value;
    }

    // Contains check
    bool contains(const std::string& key) const {
        return m_object.find(key) != m_object.end();
    }

    // Items iterator (simplified)
    auto items() const {
        return m_object;
    }

    // Static object creation
    static json object() {
        json j;
        j.m_type = object_type;
        return j;
    }

    // Dump to string (simplified)
    std::string dump(int indent = -1) const {
        (void)indent; // Suppress unused parameter warning
        return "{}"; // Simplified
    }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const json& j) {
        os << j.dump();
        return os;
    }
};

} // namespace nlohmann
