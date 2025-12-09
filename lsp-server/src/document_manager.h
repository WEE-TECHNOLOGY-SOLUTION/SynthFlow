#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

// Document content tracking
class Document {
public:
    std::string uri;
    std::string content;
    int version;
    std::vector<std::string> lines;
    
    Document() : version(0) {}
    Document(const std::string& uri_, const std::string& content_, int ver = 1)
        : uri(uri_), content(content_), version(ver) {
        updateLines();
    }
    
    void updateContent(const std::string& newContent, int newVersion) {
        content = newContent;
        version = newVersion;
        updateLines();
    }
    
    void updateLines() {
        lines.clear();
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            // Remove trailing \r if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push_back(line);
        }
    }
    
    std::string getLine(size_t lineNumber) const {
        if (lineNumber < lines.size()) {
            return lines[lineNumber];
        }
        return "";
    }
    
    size_t lineCount() const {
        return lines.size();
    }
    
    // Get word at position
    std::string getWordAt(size_t line, size_t character) const {
        if (line >= lines.size()) return "";
        const std::string& lineText = lines[line];
        if (character >= lineText.length()) return "";
        
        // Find word boundaries
        size_t start = character;
        size_t end = character;
        
        while (start > 0 && isWordChar(lineText[start - 1])) {
            start--;
        }
        while (end < lineText.length() && isWordChar(lineText[end])) {
            end++;
        }
        
        if (start >= end) return "";
        return lineText.substr(start, end - start);
    }
    
private:
    static bool isWordChar(char c) {
        return std::isalnum(c) || c == '_';
    }
};

// Document manager - tracks open documents
class DocumentManager {
private:
    std::map<std::string, std::shared_ptr<Document>> documents;
    
public:
    void openDocument(const std::string& uri, const std::string& content, int version = 1) {
        documents[uri] = std::make_shared<Document>(uri, content, version);
    }
    
    void updateDocument(const std::string& uri, const std::string& content, int version) {
        auto it = documents.find(uri);
        if (it != documents.end()) {
            it->second->updateContent(content, version);
        } else {
            openDocument(uri, content, version);
        }
    }
    
    void closeDocument(const std::string& uri) {
        documents.erase(uri);
    }
    
    std::shared_ptr<Document> getDocument(const std::string& uri) {
        auto it = documents.find(uri);
        if (it != documents.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    bool hasDocument(const std::string& uri) const {
        return documents.find(uri) != documents.end();
    }
    
    std::vector<std::string> getOpenDocuments() const {
        std::vector<std::string> result;
        for (const auto& pair : documents) {
            result.push_back(pair.first);
        }
        return result;
    }
};

// Diagnostic severity
enum class DiagnosticSeverity {
    ERROR = 1,
    WARNING = 2,
    INFORMATION = 3,
    HINT = 4
};

// Position in document
struct Position {
    int line;
    int character;
    
    Position() : line(0), character(0) {}
    Position(int l, int c) : line(l), character(c) {}
};

// Range in document
struct Range {
    Position start;
    Position end;
    
    Range() {}
    Range(Position s, Position e) : start(s), end(e) {}
    Range(int startLine, int startChar, int endLine, int endChar)
        : start(startLine, startChar), end(endLine, endChar) {}
};

// Diagnostic message
struct Diagnostic {
    Range range;
    DiagnosticSeverity severity;
    std::string code;
    std::string source;
    std::string message;
    
    Diagnostic() : severity(DiagnosticSeverity::ERROR), source("synthflow") {}
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{";
        oss << "\"range\":{";
        oss << "\"start\":{\"line\":" << range.start.line << ",\"character\":" << range.start.character << "},";
        oss << "\"end\":{\"line\":" << range.end.line << ",\"character\":" << range.end.character << "}";
        oss << "},";
        oss << "\"severity\":" << static_cast<int>(severity) << ",";
        oss << "\"source\":\"" << source << "\",";
        oss << "\"message\":\"" << escapeJson(message) << "\"";
        if (!code.empty()) {
            oss << ",\"code\":\"" << code << "\"";
        }
        oss << "}";
        return oss.str();
    }
    
private:
    static std::string escapeJson(const std::string& s) {
        std::ostringstream oss;
        for (char c : s) {
            if (c == '"') oss << "\\\"";
            else if (c == '\\') oss << "\\\\";
            else if (c == '\n') oss << "\\n";
            else if (c == '\r') oss << "\\r";
            else if (c == '\t') oss << "\\t";
            else oss << c;
        }
        return oss.str();
    }
};

// Completion item kinds
enum class CompletionItemKind {
    TEXT = 1,
    METHOD = 2,
    FUNCTION = 3,
    CONSTRUCTOR = 4,
    FIELD = 5,
    VARIABLE = 6,
    CLASS = 7,
    INTERFACE = 8,
    MODULE = 9,
    PROPERTY = 10,
    UNIT = 11,
    VALUE = 12,
    ENUM = 13,
    KEYWORD = 14,
    SNIPPET = 15,
    COLOR = 16,
    FILE = 17,
    REFERENCE = 18,
    FOLDER = 19,
    ENUM_MEMBER = 20,
    CONSTANT = 21,
    STRUCT = 22,
    EVENT = 23,
    OPERATOR = 24,
    TYPE_PARAMETER = 25
};

// Completion item
struct CompletionItem {
    std::string label;
    CompletionItemKind kind;
    std::string detail;
    std::string documentation;
    std::string insertText;
    
    CompletionItem() : kind(CompletionItemKind::TEXT) {}
    CompletionItem(const std::string& lbl, CompletionItemKind k, const std::string& det = "")
        : label(lbl), kind(k), detail(det) {}
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{";
        oss << "\"label\":\"" << label << "\",";
        oss << "\"kind\":" << static_cast<int>(kind);
        if (!detail.empty()) {
            oss << ",\"detail\":\"" << detail << "\"";
        }
        if (!documentation.empty()) {
            oss << ",\"documentation\":\"" << documentation << "\"";
        }
        if (!insertText.empty() && insertText != label) {
            oss << ",\"insertText\":\"" << insertText << "\"";
        }
        oss << "}";
        return oss.str();
    }
};

// Hover information
struct Hover {
    std::string contents;  // Markdown content
    Range range;
    bool hasRange;
    
    Hover() : hasRange(false) {}
    Hover(const std::string& c) : contents(c), hasRange(false) {}
    Hover(const std::string& c, Range r) : contents(c), range(r), hasRange(true) {}
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{";
        oss << "\"contents\":{\"kind\":\"markdown\",\"value\":\"" << escapeJson(contents) << "\"}";
        if (hasRange) {
            oss << ",\"range\":{";
            oss << "\"start\":{\"line\":" << range.start.line << ",\"character\":" << range.start.character << "},";
            oss << "\"end\":{\"line\":" << range.end.line << ",\"character\":" << range.end.character << "}";
            oss << "}";
        }
        oss << "}";
        return oss.str();
    }
    
private:
    static std::string escapeJson(const std::string& s) {
        std::ostringstream oss;
        for (char c : s) {
            if (c == '"') oss << "\\\"";
            else if (c == '\\') oss << "\\\\";
            else if (c == '\n') oss << "\\n";
            else if (c == '\r') oss << "\\r";
            else if (c == '\t') oss << "\\t";
            else oss << c;
        }
        return oss.str();
    }
};
