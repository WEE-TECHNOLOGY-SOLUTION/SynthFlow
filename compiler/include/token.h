#pragma once
#include <string>
#include <variant>
#include <cstdint>

enum class TokenType {
    // Keywords
    KW_FN, KW_LET, KW_IF, KW_ELSE, KW_WHILE, KW_RETURN,
    KW_TYPE, KW_STRUCT, KW_ENUM, KW_INTENT, KW_BREAK, KW_CONTINUE, KW_FOR,
    KW_ARRAY, // Keyword for array type
    KW_MATCH, // Match expression
    
    // SADK Keywords (Agent Development Kit)
    KW_IMPORT,   // import statement
    KW_FROM,     // from clause in import
    KW_AS,       // alias in import
    KW_SELF,     // self reference in methods
    KW_ASYNC,    // async function
    KW_AWAIT,    // await expression
    KW_EXTENDS,  // struct inheritance
    KW_MAP,      // map type (like object/dict)
    
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
    INTERPOLATED_STRING, // String with ${} interpolation
    
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    PLUS_PLUS, MINUS_MINUS, // ++ and --
    PLUS_EQ, MINUS_EQ, STAR_EQ, SLASH_EQ, // +=, -=, *=, /=
    EQ, NE, LT, GT, LE, GE,
    AND, OR, NOT,
    ASSIGN, ARROW, FAT_ARROW, DOT, COMMA, COLON, SEMICOLON,
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