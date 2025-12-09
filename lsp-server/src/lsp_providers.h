#pragma once
#include "document_manager.h"
#include "../../compiler/include/lexer.h"
#include "../../compiler/include/parser.h"
#include <vector>
#include <string>
#include <set>

// Diagnostics provider - analyzes documents for errors
class DiagnosticsProvider {
private:
    // Keywords for SynthFlow
    std::set<std::string> keywords = {
        "fn", "let", "const", "if", "else", "while", "for", "return",
        "break", "continue", "match", "try", "catch", "null", "true", "false",
        "int", "float", "string", "bool", "struct", "enum", "type"
    };
    
    // Built-in functions
    std::set<std::string> builtins = {
        "print", "input", "len", "str", "int", "float", "read_file", "write_file"
    };
    
public:
    std::vector<Diagnostic> analyze(const std::string& content) {
        std::vector<Diagnostic> diagnostics;
        
        // Try to lex the content
        try {
            Lexer lexer(content);
            auto tokens = lexer.tokenize();
            
            // Check for invalid tokens
            for (const auto& token : tokens) {
                if (token.type == TokenType::INVALID) {
                    Diagnostic diag;
                    diag.range = Range(
                        static_cast<int>(token.line) - 1, 
                        static_cast<int>(token.column) - 1,
                        static_cast<int>(token.line) - 1, 
                        static_cast<int>(token.column) - 1 + static_cast<int>(token.lexeme.length())
                    );
                    diag.severity = DiagnosticSeverity::ERROR;
                    diag.code = "E1000";
                    diag.message = "Unexpected character: '" + token.lexeme + "'";
                    diagnostics.push_back(diag);
                }
            }
            
            // Try to parse
            try {
                Parser parser(tokens);
                auto ast = parser.parse();
                // If we get here, parsing succeeded
            } catch (const std::exception& e) {
                // Parser error
                Diagnostic diag;
                diag.range = Range(0, 0, 0, 10);  // Default range
                diag.severity = DiagnosticSeverity::ERROR;
                diag.code = "E2000";
                diag.message = std::string("Parse error: ") + e.what();
                diagnostics.push_back(diag);
            }
            
        } catch (const std::exception& e) {
            // Lexer error
            Diagnostic diag;
            diag.range = Range(0, 0, 0, 10);
            diag.severity = DiagnosticSeverity::ERROR;
            diag.code = "E1001";
            diag.message = std::string("Lexer error: ") + e.what();
            diagnostics.push_back(diag);
        }
        
        // Check for common issues
        checkCommonIssues(content, diagnostics);
        
        return diagnostics;
    }
    
private:
    void checkCommonIssues(const std::string& content, std::vector<Diagnostic>& diagnostics) {
        std::istringstream stream(content);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(stream, line)) {
            // Check for trailing whitespace
            if (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
                Diagnostic diag;
                diag.range = Range(lineNum, static_cast<int>(line.length()) - 1, 
                                   lineNum, static_cast<int>(line.length()));
                diag.severity = DiagnosticSeverity::HINT;
                diag.code = "W0001";
                diag.message = "Trailing whitespace";
                diagnostics.push_back(diag);
            }
            
            // Check for very long lines
            if (line.length() > 120) {
                Diagnostic diag;
                diag.range = Range(lineNum, 120, lineNum, static_cast<int>(line.length()));
                diag.severity = DiagnosticSeverity::INFORMATION;
                diag.code = "W0002";
                diag.message = "Line exceeds 120 characters";
                diagnostics.push_back(diag);
            }
            
            // Check for TODO/FIXME comments
            size_t todoPos = line.find("TODO");
            if (todoPos != std::string::npos) {
                Diagnostic diag;
                diag.range = Range(lineNum, static_cast<int>(todoPos), 
                                   lineNum, static_cast<int>(todoPos) + 4);
                diag.severity = DiagnosticSeverity::INFORMATION;
                diag.code = "I0001";
                diag.message = "TODO comment found";
                diagnostics.push_back(diag);
            }
            
            size_t fixmePos = line.find("FIXME");
            if (fixmePos != std::string::npos) {
                Diagnostic diag;
                diag.range = Range(lineNum, static_cast<int>(fixmePos), 
                                   lineNum, static_cast<int>(fixmePos) + 5);
                diag.severity = DiagnosticSeverity::WARNING;
                diag.code = "W0003";
                diag.message = "FIXME comment found";
                diagnostics.push_back(diag);
            }
            
            lineNum++;
        }
    }
};

// Completion provider - provides code completions
class CompletionProvider {
private:
    std::vector<CompletionItem> keywordCompletions;
    std::vector<CompletionItem> builtinCompletions;
    std::vector<CompletionItem> snippetCompletions;
    
public:
    CompletionProvider() {
        initializeCompletions();
    }
    
    std::vector<CompletionItem> getCompletions(
        const Document& doc, 
        const Position& position,
        const std::string& triggerCharacter = ""
    ) {
        std::vector<CompletionItem> results;
        
        // Get the word being typed
        std::string wordAtCursor = doc.getWordAt(position.line, position.character);
        
        // Add keyword completions
        for (const auto& item : keywordCompletions) {
            if (wordAtCursor.empty() || startsWith(item.label, wordAtCursor)) {
                results.push_back(item);
            }
        }
        
        // Add builtin completions
        for (const auto& item : builtinCompletions) {
            if (wordAtCursor.empty() || startsWith(item.label, wordAtCursor)) {
                results.push_back(item);
            }
        }
        
        // Add snippet completions
        for (const auto& item : snippetCompletions) {
            if (wordAtCursor.empty() || startsWith(item.label, wordAtCursor)) {
                results.push_back(item);
            }
        }
        
        return results;
    }
    
private:
    void initializeCompletions() {
        // Keywords
        keywordCompletions.push_back(CompletionItem("fn", CompletionItemKind::KEYWORD, "Function declaration"));
        keywordCompletions.push_back(CompletionItem("let", CompletionItemKind::KEYWORD, "Variable declaration"));
        keywordCompletions.push_back(CompletionItem("const", CompletionItemKind::KEYWORD, "Constant declaration"));
        keywordCompletions.push_back(CompletionItem("if", CompletionItemKind::KEYWORD, "If statement"));
        keywordCompletions.push_back(CompletionItem("else", CompletionItemKind::KEYWORD, "Else clause"));
        keywordCompletions.push_back(CompletionItem("while", CompletionItemKind::KEYWORD, "While loop"));
        keywordCompletions.push_back(CompletionItem("for", CompletionItemKind::KEYWORD, "For loop"));
        keywordCompletions.push_back(CompletionItem("return", CompletionItemKind::KEYWORD, "Return statement"));
        keywordCompletions.push_back(CompletionItem("break", CompletionItemKind::KEYWORD, "Break from loop"));
        keywordCompletions.push_back(CompletionItem("continue", CompletionItemKind::KEYWORD, "Continue to next iteration"));
        keywordCompletions.push_back(CompletionItem("match", CompletionItemKind::KEYWORD, "Match expression"));
        keywordCompletions.push_back(CompletionItem("try", CompletionItemKind::KEYWORD, "Try block"));
        keywordCompletions.push_back(CompletionItem("catch", CompletionItemKind::KEYWORD, "Catch block"));
        keywordCompletions.push_back(CompletionItem("null", CompletionItemKind::KEYWORD, "Null value"));
        keywordCompletions.push_back(CompletionItem("true", CompletionItemKind::KEYWORD, "Boolean true"));
        keywordCompletions.push_back(CompletionItem("false", CompletionItemKind::KEYWORD, "Boolean false"));
        
        // Type keywords
        keywordCompletions.push_back(CompletionItem("int", CompletionItemKind::KEYWORD, "Integer type"));
        keywordCompletions.push_back(CompletionItem("float", CompletionItemKind::KEYWORD, "Float type"));
        keywordCompletions.push_back(CompletionItem("string", CompletionItemKind::KEYWORD, "String type"));
        keywordCompletions.push_back(CompletionItem("bool", CompletionItemKind::KEYWORD, "Boolean type"));
        keywordCompletions.push_back(CompletionItem("struct", CompletionItemKind::KEYWORD, "Struct declaration"));
        
        // Built-in functions
        auto printItem = CompletionItem("print", CompletionItemKind::FUNCTION, "Print to stdout");
        printItem.documentation = "Prints values to standard output.";
        printItem.insertText = "print($1)";
        builtinCompletions.push_back(printItem);
        
        auto inputItem = CompletionItem("input", CompletionItemKind::FUNCTION, "Read from stdin");
        inputItem.documentation = "Reads a line from standard input with optional prompt.";
        inputItem.insertText = "input(\"$1\")";
        builtinCompletions.push_back(inputItem);
        
        auto lenItem = CompletionItem("len", CompletionItemKind::FUNCTION, "Get length");
        lenItem.documentation = "Returns the length of a string or array.";
        lenItem.insertText = "len($1)";
        builtinCompletions.push_back(lenItem);
        
        builtinCompletions.push_back(CompletionItem("str", CompletionItemKind::FUNCTION, "Convert to string"));
        builtinCompletions.push_back(CompletionItem("int", CompletionItemKind::FUNCTION, "Convert to integer"));
        builtinCompletions.push_back(CompletionItem("float", CompletionItemKind::FUNCTION, "Convert to float"));
        builtinCompletions.push_back(CompletionItem("read_file", CompletionItemKind::FUNCTION, "Read file contents"));
        builtinCompletions.push_back(CompletionItem("write_file", CompletionItemKind::FUNCTION, "Write to file"));
        
        // Snippets
        auto fnSnippet = CompletionItem("fn", CompletionItemKind::SNIPPET, "Function definition");
        fnSnippet.insertText = "fn ${1:name}(${2:params}) {\n    ${3:// body}\n}";
        fnSnippet.label = "fn (snippet)";
        snippetCompletions.push_back(fnSnippet);
        
        auto ifSnippet = CompletionItem("if", CompletionItemKind::SNIPPET, "If statement");
        ifSnippet.insertText = "if (${1:condition}) {\n    ${2:// body}\n}";
        ifSnippet.label = "if (snippet)";
        snippetCompletions.push_back(ifSnippet);
        
        auto forSnippet = CompletionItem("for", CompletionItemKind::SNIPPET, "For loop");
        forSnippet.insertText = "for (let ${1:i} = 0; ${1:i} < ${2:n}; ${1:i} = ${1:i} + 1) {\n    ${3:// body}\n}";
        forSnippet.label = "for (snippet)";
        snippetCompletions.push_back(forSnippet);
        
        auto whileSnippet = CompletionItem("while", CompletionItemKind::SNIPPET, "While loop");
        whileSnippet.insertText = "while (${1:condition}) {\n    ${2:// body}\n}";
        whileSnippet.label = "while (snippet)";
        snippetCompletions.push_back(whileSnippet);
        
        auto trySnippet = CompletionItem("try", CompletionItemKind::SNIPPET, "Try-catch block");
        trySnippet.insertText = "try {\n    ${1:// risky code}\n} catch (${2:e}) {\n    ${3:// handle error}\n}";
        trySnippet.label = "try (snippet)";
        snippetCompletions.push_back(trySnippet);
    }
    
    static bool startsWith(const std::string& str, const std::string& prefix) {
        if (prefix.length() > str.length()) return false;
        for (size_t i = 0; i < prefix.length(); ++i) {
            if (std::tolower(str[i]) != std::tolower(prefix[i])) return false;
        }
        return true;
    }
};

// Hover provider - provides hover information
class HoverProvider {
private:
    std::map<std::string, std::string> keywordDocs;
    std::map<std::string, std::string> builtinDocs;
    
public:
    HoverProvider() {
        initializeDocs();
    }
    
    Hover getHover(const Document& doc, const Position& position) {
        std::string word = doc.getWordAt(position.line, position.character);
        if (word.empty()) {
            return Hover();
        }
        
        // Check keywords
        auto kwIt = keywordDocs.find(word);
        if (kwIt != keywordDocs.end()) {
            return Hover("**" + word + "** (keyword)\\n\\n" + kwIt->second);
        }
        
        // Check builtins
        auto builtinIt = builtinDocs.find(word);
        if (builtinIt != builtinDocs.end()) {
            return Hover("**" + word + "** (built-in function)\\n\\n" + builtinIt->second);
        }
        
        // Default - just show the word
        return Hover();
    }
    
private:
    void initializeDocs() {
        // Keywords
        keywordDocs["fn"] = "Declares a function.\\n\\nSyntax: `fn name(params) { body }`";
        keywordDocs["let"] = "Declares a mutable variable.\\n\\nSyntax: `let name = value`";
        keywordDocs["const"] = "Declares an immutable constant.\\n\\nSyntax: `const NAME = value`";
        keywordDocs["if"] = "Conditional statement.\\n\\nSyntax: `if (condition) { ... }`";
        keywordDocs["else"] = "Alternative branch for if statement.";
        keywordDocs["while"] = "Loop while condition is true.\\n\\nSyntax: `while (condition) { ... }`";
        keywordDocs["for"] = "For loop with initializer, condition, and update.\\n\\nSyntax: `for (init; cond; update) { ... }`";
        keywordDocs["return"] = "Return a value from a function.";
        keywordDocs["break"] = "Exit the current loop immediately.";
        keywordDocs["continue"] = "Skip to the next iteration of the loop.";
        keywordDocs["match"] = "Pattern matching expression.\\n\\nSyntax: `match value { pattern => result, ... }`";
        keywordDocs["try"] = "Begin a try-catch block for error handling.";
        keywordDocs["catch"] = "Handle errors from a try block.";
        keywordDocs["null"] = "Represents the absence of a value.";
        keywordDocs["true"] = "Boolean true value.";
        keywordDocs["false"] = "Boolean false value.";
        
        // Built-ins
        builtinDocs["print"] = "Prints values to standard output.\\n\\n`print(value, ...)`";
        builtinDocs["input"] = "Reads a line from standard input.\\n\\n`input(prompt) -> string`";
        builtinDocs["len"] = "Returns the length of a string or array.\\n\\n`len(value) -> int`";
        builtinDocs["str"] = "Converts a value to a string.\\n\\n`str(value) -> string`";
        builtinDocs["int"] = "Converts a value to an integer.\\n\\n`int(value) -> int`";
        builtinDocs["float"] = "Converts a value to a float.\\n\\n`float(value) -> float`";
        builtinDocs["read_file"] = "Reads the contents of a file.\\n\\n`read_file(path) -> string`";
        builtinDocs["write_file"] = "Writes content to a file.\\n\\n`write_file(path, content)`";
    }
};
