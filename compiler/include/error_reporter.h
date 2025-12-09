#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// Error severity levels
enum class ErrorSeverity {
    HINT,
    WARNING,
    ERROR,
    FATAL
};

// Source location for precise error reporting
struct SourceLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t length;
    
    SourceLocation() : filename(""), line(0), column(0), length(0) {}
    SourceLocation(const std::string& file, size_t ln, size_t col, size_t len = 1)
        : filename(file), line(ln), column(col), length(len) {}
    
    std::string toString() const {
        std::ostringstream oss;
        if (!filename.empty()) oss << filename << ":";
        oss << line << ":" << column;
        return oss.str();
    }
};

// Error code categories
enum class ErrorCode {
    // Lexer errors (1000-1999)
    LEX_UNEXPECTED_CHAR = 1000,
    LEX_UNTERMINATED_STRING = 1001,
    LEX_INVALID_NUMBER = 1002,
    LEX_INVALID_ESCAPE = 1003,
    
    // Parser errors (2000-2999)
    PARSE_UNEXPECTED_TOKEN = 2000,
    PARSE_EXPECTED_EXPRESSION = 2001,
    PARSE_EXPECTED_STATEMENT = 2002,
    PARSE_EXPECTED_IDENTIFIER = 2003,
    PARSE_EXPECTED_TYPE = 2004,
    PARSE_MISSING_SEMICOLON = 2005,
    PARSE_MISSING_PAREN = 2006,
    PARSE_MISSING_BRACE = 2007,
    PARSE_MISSING_BRACKET = 2008,
    
    // Semantic errors (3000-3999)
    SEM_UNDEFINED_VARIABLE = 3000,
    SEM_UNDEFINED_FUNCTION = 3001,
    SEM_DUPLICATE_DEFINITION = 3002,
    SEM_TYPE_MISMATCH = 3003,
    SEM_CONST_REASSIGNMENT = 3004,
    SEM_BREAK_OUTSIDE_LOOP = 3005,
    SEM_CONTINUE_OUTSIDE_LOOP = 3006,
    SEM_RETURN_OUTSIDE_FUNCTION = 3007,
    SEM_WRONG_ARG_COUNT = 3008,
    
    // Runtime errors (4000-4999)
    RT_DIVISION_BY_ZERO = 4000,
    RT_INDEX_OUT_OF_BOUNDS = 4001,
    RT_NULL_REFERENCE = 4002,
    RT_TYPE_ERROR = 4003,
    RT_STACK_OVERFLOW = 4004,
    
    // Generic
    UNKNOWN_ERROR = 9999
};

// A single compiler/runtime error
struct CompilerError {
    ErrorSeverity severity;
    ErrorCode code;
    std::string message;
    SourceLocation location;
    std::string sourceLine;  // The actual line of source code for context
    std::string hint;        // Optional hint for fixing
    
    CompilerError(ErrorSeverity sev, ErrorCode c, const std::string& msg,
                  const SourceLocation& loc = SourceLocation())
        : severity(sev), code(c), message(msg), location(loc) {}
};

// Error reporter class - singleton pattern
class ErrorReporter {
private:
    std::vector<CompilerError> errors;
    std::vector<CompilerError> warnings;
    std::string currentSource;
    std::string currentFile;
    bool useColors;
    int maxErrors;
    
    // ANSI color codes
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string CYAN = "\033[36m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    
    std::string getSourceLine(size_t lineNum) const {
        if (currentSource.empty() || lineNum == 0) return "";
        
        std::istringstream stream(currentSource);
        std::string line;
        size_t currentLine = 0;
        while (std::getline(stream, line)) {
            currentLine++;
            if (currentLine == lineNum) return line;
        }
        return "";
    }
    
    std::string severityToString(ErrorSeverity sev) const {
        switch (sev) {
            case ErrorSeverity::HINT: return "hint";
            case ErrorSeverity::WARNING: return "warning";
            case ErrorSeverity::ERROR: return "error";
            case ErrorSeverity::FATAL: return "fatal";
            default: return "unknown";
        }
    }
    
    std::string severityColor(ErrorSeverity sev) const {
        if (!useColors) return "";
        switch (sev) {
            case ErrorSeverity::HINT: return CYAN;
            case ErrorSeverity::WARNING: return YELLOW;
            case ErrorSeverity::ERROR: return RED;
            case ErrorSeverity::FATAL: return RED + BOLD;
            default: return "";
        }
    }

public:
    ErrorReporter(bool colors = true, int maxErrs = 50)
        : useColors(colors), maxErrors(maxErrs) {}
    
    void setSource(const std::string& source, const std::string& filename = "<stdin>") {
        currentSource = source;
        currentFile = filename;
    }
    
    void report(ErrorSeverity severity, ErrorCode code, const std::string& message,
                const SourceLocation& location = SourceLocation(),
                const std::string& hint = "") {
        CompilerError err(severity, code, message, location);
        err.sourceLine = getSourceLine(location.line);
        err.hint = hint;
        
        if (severity == ErrorSeverity::WARNING || severity == ErrorSeverity::HINT) {
            warnings.push_back(err);
        } else {
            errors.push_back(err);
        }
        
        // Print immediately
        printError(err);
        
        // Stop if too many errors
        if (errors.size() >= static_cast<size_t>(maxErrors)) {
            std::cerr << "\n" << (useColors ? RED : "") 
                      << "Too many errors, stopping compilation."
                      << (useColors ? RESET : "") << "\n";
        }
    }
    
    void printError(const CompilerError& err) const {
        std::ostringstream oss;
        
        // Location and severity
        if (useColors) oss << BOLD;
        oss << err.location.toString() << ": ";
        oss << severityColor(err.severity);
        oss << severityToString(err.severity);
        if (useColors) oss << RESET << BOLD;
        oss << ": " << err.message;
        if (useColors) oss << RESET;
        oss << " [E" << static_cast<int>(err.code) << "]";
        oss << "\n";
        
        // Source line with caret
        if (!err.sourceLine.empty()) {
            if (useColors) oss << DIM;
            oss << "  " << err.location.line << " | ";
            if (useColors) oss << RESET;
            oss << err.sourceLine << "\n";
            
            // Caret pointing to error
            oss << "    | ";
            for (size_t i = 1; i < err.location.column; i++) {
                oss << " ";
            }
            oss << severityColor(err.severity);
            oss << "^";
            for (size_t i = 1; i < err.location.length; i++) {
                oss << "~";
            }
            if (useColors) oss << RESET;
            oss << "\n";
        }
        
        // Hint
        if (!err.hint.empty()) {
            if (useColors) oss << CYAN;
            oss << "  = hint: " << err.hint;
            if (useColors) oss << RESET;
            oss << "\n";
        }
        
        std::cerr << oss.str();
    }
    
    // Convenience methods
    void error(const std::string& msg, const SourceLocation& loc = SourceLocation()) {
        report(ErrorSeverity::ERROR, ErrorCode::UNKNOWN_ERROR, msg, loc);
    }
    
    void error(ErrorCode code, const std::string& msg, const SourceLocation& loc = SourceLocation()) {
        report(ErrorSeverity::ERROR, code, msg, loc);
    }
    
    void warning(const std::string& msg, const SourceLocation& loc = SourceLocation()) {
        report(ErrorSeverity::WARNING, ErrorCode::UNKNOWN_ERROR, msg, loc);
    }
    
    void hint(const std::string& msg, const SourceLocation& loc = SourceLocation()) {
        report(ErrorSeverity::HINT, ErrorCode::UNKNOWN_ERROR, msg, loc);
    }
    
    void fatal(const std::string& msg, const SourceLocation& loc = SourceLocation()) {
        report(ErrorSeverity::FATAL, ErrorCode::UNKNOWN_ERROR, msg, loc);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    size_t errorCount() const { return errors.size(); }
    size_t warningCount() const { return warnings.size(); }
    
    void printSummary() const {
        if (errors.empty() && warnings.empty()) {
            if (useColors) std::cerr << "\033[32m";
            std::cerr << "Compilation successful.\n";
            if (useColors) std::cerr << RESET;
        } else {
            std::cerr << "\nCompilation ";
            if (!errors.empty()) {
                if (useColors) std::cerr << RED;
                std::cerr << "failed";
                if (useColors) std::cerr << RESET;
            } else {
                if (useColors) std::cerr << "\033[32m";
                std::cerr << "succeeded";
                if (useColors) std::cerr << RESET;
            }
            std::cerr << " with " << errors.size() << " error(s) and " 
                      << warnings.size() << " warning(s).\n";
        }
    }
    
    void clear() {
        errors.clear();
        warnings.clear();
    }
    
    const std::vector<CompilerError>& getErrors() const { return errors; }
    const std::vector<CompilerError>& getWarnings() const { return warnings; }
};

// Global error reporter instance
inline ErrorReporter& getErrorReporter() {
    static ErrorReporter instance;
    return instance;
}
