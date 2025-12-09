#pragma once
#include <string>
#include <variant>
#include <cstdint>

enum class TokenType {
    // Keywords
    KW_FN, KW_LET, KW_IF, KW_ELSE, KW_WHILE, KW_RETURN,
    KW_TYPE, KW_STRUCT, KW_ENUM, KW_INTENT, KW_BREAK, KW_CONTINUE, KW_FOR,
    KW_ARRAY, // Keyword for array type
    
    // Safety Feature Keywords
    KW_CONST,  // Immutable variables
    KW_TRY,    // Error handling
    KW_CATCH,  // Error handling
    KW_NULL,   // Null literal
    
    // Type Keywords (for type annotations)
    KW_INT, KW_FLOAT, KW_STRING, KW_BOOL,
    
    // Identifiers and Literals
    IDENTIFIER, INTEGER, FLOAT, STRING, BOOLEAN,
    ARRAY_LITERAL, // Token type for array literals
    
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQ, NE, LT, GT, LE, GE,
    AND, OR, NOT,
    ASSIGN, ARROW, DOT, COMMA, COLON, SEMICOLON,
    QUESTION,  // For nullable types (?)
    
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    
    // Special
    NEWLINE, INDENT, DEDENT, EOF_TOKEN, INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<int64_t, double, std::string, bool> value;
    size_t line;
    size_t column;
    
    Token(TokenType t, std::string lex, size_t ln, size_t col)
        : type(t), lexeme(std::move(lex)), line(ln), column(col) {}
};