In the house#pragma once
#include <string>
#include <variant>

enum class TokenType {
    // Keywords
    KW_FN, KW_LET, KW_IF, KW_ELSE, KW_WHILE, KW_RETURN,
    KW_TYPE, KW_STRUCT, KW_ENUM, KW_INTENT,
    
    // Identifiers and Literals
    IDENTIFIER, INTEGER, FLOAT, STRING, BOOLEAN,
    
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQ, NE, LT, GT, LE, GE,
    AND, OR, NOT,
    ASSIGN, ARROW, DOT, COMMA, COLON, SEMICOLON,
    
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