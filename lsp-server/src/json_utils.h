#pragma once
#include <string>
#include <sstream>
#include <map>
#include <vector>

// Very minimal JSON serializer/parser
namespace json {

    std::string quote(const std::string& s) {
        std::stringstream ss;
        ss << "\"";
        for (char c : s) {
            if (c == '\"') ss << "\\\"";
            else if (c == '\\') ss << "\\\\";
            else if (c == '\n') ss << "\\n";
            else if (c == '\r') ss << "\\r";
            else if (c == '\t') ss << "\\t";
            else ss << c;
        }
        ss << "\"";
        return ss.str();
    }

    struct Value {
        std::string string_val;
        bool is_string = false;
        long long int_val;
        bool is_int = false;

        Value() {}
        Value(const std::string& s) : string_val(s), is_string(true) {}
        Value(const char* s) : string_val(s), is_string(true) {}
        Value(long long i) : int_val(i), is_int(true) {}
    };

    // Helper to extract method from JSON-RPC
    // Look for "method":"..."
    std::string get_method(const std::string& json_str) {
        size_t pos = json_str.find("\"method\"");
        if (pos == std::string::npos) return "";
        size_t start = json_str.find("\"", pos + 8); // Skip "method"
        if (start == std::string::npos) return "";
        start = json_str.find("\"", start + 1); // Start of value
        if (start == std::string::npos) return "";
        size_t end = json_str.find("\"", start + 1);
        if (end == std::string::npos) return "";
        return json_str.substr(start + 1, end - start - 1);
    }
}
