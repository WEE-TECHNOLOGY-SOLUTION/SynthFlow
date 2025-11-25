#include "lexer.h"
#include <cctype>
#include <unordered_map>

// Initialize keywords map
const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"fn", TokenType::KW_FN},
    {"let", TokenType::KW_LET},
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"return", TokenType::KW_RETURN},
    {"type", TokenType::KW_TYPE},
    {"struct", TokenType::KW_STRUCT},
    {"enum", TokenType::KW_ENUM},
    {"intent", TokenType::KW_INTENT},
    {"break", TokenType::KW_BREAK},
    {"continue", TokenType::KW_CONTINUE},
    {"true", TokenType::BOOLEAN},
    {"false", TokenType::BOOLEAN}
};

char Lexer::current() {
    return pos < source.length() ? source[pos] : '\0';
}

char Lexer::peek(int offset) {
    return (pos + offset) < source.length() ? source[pos + offset] : '\0';
}

void Lexer::advance() {
    if (current() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
}

void Lexer::skipWhitespace() {
    while (std::isspace(current()) && current() != '\n') {
        advance();
    }
}

Token Lexer::makeToken(TokenType type, std::string lexeme) {
    return Token(type, std::move(lexeme), line, column - lexeme.length());
}

Token Lexer::lexNumber() {
    std::string num;
    bool isFloat = false;
    
    while (std::isdigit(current()) || current() == '.') {
        if (current() == '.') {
            if (isFloat) break; // Second dot, stop
            isFloat = true;
        }
        num += current();
        advance();
    }
    
    Token token = makeToken(isFloat ? TokenType::FLOAT : TokenType::INTEGER, num);
    if (isFloat) {
        token.value = std::stod(num);
    } else {
        token.value = std::stoll(num);
    }
    return token;
}

Token Lexer::lexIdentifier() {
    std::string ident;
    while (std::isalnum(current()) || current() == '_') {
        ident += current();
        advance();
    }
    
    // Check if keyword
    auto it = keywords.find(ident);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    return makeToken(type, ident);
}

Token Lexer::lexString() {
    advance(); // Skip opening quote
    std::string str;
    
    while (current() != '"' && current() != '\0') {
        if (current() == '\\') {
            advance();
            switch (current()) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                default: str += current();
            }
        } else {
            str += current();
        }
        advance();
    }
    
    if (current() == '"') advance(); // Skip closing quote
    Token token = makeToken(TokenType::STRING, str);
    token.value = str;
    return token;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (current() != '\0') {
        skipWhitespace();
        if (current() == '\0') break;
        
        // Handle newlines (significant in SynthFlow)
        if (current() == '\n') {
            tokens.push_back(makeToken(TokenType::NEWLINE, "\n"));
            advance();
            continue;
        }
        
        // Handle comments
        if (current() == '#') {
            while (current() != '\n' && current() != '\0') advance();
            continue;
        }
        
        // Numbers
        if (std::isdigit(current())) {
            tokens.push_back(lexNumber());
            continue;
        }
        
        // Identifiers and keywords
        if (std::isalpha(current()) || current() == '_') {
            tokens.push_back(lexIdentifier());
            continue;
        }
        
        // Strings
        if (current() == '"') {
            tokens.push_back(lexString());
            continue;
        }
        
        // Operators and delimiters
        switch (current()) {
            case '+': 
                tokens.push_back(makeToken(TokenType::PLUS, "+")); 
                advance(); 
                break;
            case '-':
                if (peek() == '>') {
                    tokens.push_back(makeToken(TokenType::ARROW, "->"));
                    advance(); 
                    advance();
                } else {
                    tokens.push_back(makeToken(TokenType::MINUS, "-"));
                    advance();
                }
                break;
            case '*': 
                tokens.push_back(makeToken(TokenType::STAR, "*")); 
                advance(); 
                break;
            case '/': 
                tokens.push_back(makeToken(TokenType::SLASH, "/")); 
                advance(); 
                break;
            case '=':
                if (peek() == '=') {
                    tokens.push_back(makeToken(TokenType::EQ, "=="));
                    advance(); 
                    advance();
                } else {
                    tokens.push_back(makeToken(TokenType::ASSIGN, "="));
                    advance();
                }
                break;
            case '<':
                if (peek() == '=') {
                    tokens.push_back(makeToken(TokenType::LE, "<="));
                    advance(); 
                    advance();
                } else {
                    tokens.push_back(makeToken(TokenType::LT, "<"));
                    advance();
                }
                break;
            case '>':
                if (peek() == '=') {
                    tokens.push_back(makeToken(TokenType::GE, ">="));
                    advance(); 
                    advance();
                } else {
                    tokens.push_back(makeToken(TokenType::GT, ">"));
                    advance();
                }
                break;
            case '(': 
                tokens.push_back(makeToken(TokenType::LPAREN, "(")); 
                advance(); 
                break;
            case ')': 
                tokens.push_back(makeToken(TokenType::RPAREN, ")")); 
                advance(); 
                break;
            case '{': 
                tokens.push_back(makeToken(TokenType::LBRACE, "{")); 
                advance(); 
                break;
            case '}': 
                tokens.push_back(makeToken(TokenType::RBRACE, "}")); 
                advance(); 
                break;
            case '[': 
                tokens.push_back(makeToken(TokenType::LBRACKET, "[")); 
                advance(); 
                break;
            case ']': 
                tokens.push_back(makeToken(TokenType::RBRACKET, "]")); 
                advance(); 
                break;
            case ':': 
                tokens.push_back(makeToken(TokenType::COLON, ":")); 
                advance(); 
                break;
            case ',': 
                tokens.push_back(makeToken(TokenType::COMMA, ",")); 
                advance(); 
                break;
            case '.': 
                tokens.push_back(makeToken(TokenType::DOT, ".")); 
                advance(); 
                break;
            case ';': 
                tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); 
                advance(); 
                break;
            default:
                tokens.push_back(makeToken(TokenType::INVALID, std::string(1, current())));
                advance();
        }
    }
    
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, ""));
    return tokens;
}