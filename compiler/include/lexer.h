#pragma once
#include "token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
private:
    std::string source;
    size_t pos = 0;
    size_t line = 1;
    size_t column = 1;
    static const std::unordered_map<std::string, TokenType> keywords;
    
    char current();
    char peek(int offset = 1);
    void advance();
    void skipWhitespace();
    Token makeToken(TokenType type, std::string lexeme);
    Token lexNumber();
    Token lexIdentifier();
    Token lexString();

public:
    explicit Lexer(std::string src) : source(std::move(src)) {}
    std::vector<Token> tokenize();
};