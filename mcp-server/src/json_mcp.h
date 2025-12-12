#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

namespace json {

    // Helper: Escape string for JSON output
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

    // Helper: Parse string value by key
    // Extremely naive parser: looks for "key":"value" or "key": "value"
    std::string get_string(const std::string& json_str, const std::string& key) {
        std::string search_key = "\"" + key + "\"";
        size_t pos = json_str.find(search_key);
        if (pos == std::string::npos) return "";

        size_t colon = json_str.find(":", pos + search_key.length());
        if (colon == std::string::npos) return "";

        size_t start_quote = json_str.find("\"", colon + 1);
        if (start_quote == std::string::npos) return "";

        size_t end_quote = start_quote;
        while (true) {
            end_quote = json_str.find("\"", end_quote + 1);
            if (end_quote == std::string::npos) return "";
            if (json_str[end_quote - 1] != '\\') break; // Found unescaped quote
        }

        return json_str.substr(start_quote + 1, end_quote - start_quote - 1);
    }

    // Helper: Parse integer ID (assuming simple format)
    std::string get_id(const std::string& json_str) {
        size_t pos = json_str.find("\"id\"");
        if (pos == std::string::npos) return "null";
        
        size_t colon = json_str.find(":", pos + 4);
        size_t start = json_str.find_first_not_of(" \t\r\n", colon + 1);
        if (start == std::string::npos) return "null";

        if (json_str[start] == '\"') {
            // String ID
            size_t end = json_str.find("\"", start + 1);
            return json_str.substr(start, end - start + 1); 
        } else {
            // Numeric ID
            size_t end = json_str.find_first_not_of("0123456789", start);
            return json_str.substr(start, end - start);
        }
    }
}
