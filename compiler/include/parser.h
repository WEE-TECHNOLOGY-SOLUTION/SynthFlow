#pragma once
#include "ast.h"
#include "token.h"
#include <vector>
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    
    Token& peek(int offset = 0);
    Token& advance();
    bool match(TokenType type);
    bool isAtEnd();
    
    // Parsing methods
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignment();
    std::unique_ptr<Expression> parseLogicalOr();    // Added for && support
    std::unique_ptr<Expression> parseLogicalAnd();   // Added for || support
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();
    std::unique_ptr<Expression> parseArrayLiteral();
    std::unique_ptr<Expression> parseIndexExpression(std::unique_ptr<Expression> array);
    
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseExpressionStatement();
    std::unique_ptr<Statement> parseVariableDeclaration();
    std::unique_ptr<Statement> parseConstDeclaration();
    std::unique_ptr<Statement> parseFunctionDeclaration();
    std::unique_ptr<Statement> parseBlockStatement();
    std::unique_ptr<Statement> parseIfStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<Statement> parseForStatement();
    std::unique_ptr<Statement> parseBreakStatement();
    std::unique_ptr<Statement> parseContinueStatement();
    std::unique_ptr<Statement> parseReturnStatement();
    std::unique_ptr<Statement> parseTryStatement();
    
    // SADK parsing methods (Agent Development Kit)
    std::unique_ptr<Expression> parseMapLiteral();
    std::unique_ptr<Expression> parseCallOrMemberExpression(std::unique_ptr<Expression> expr);
    std::unique_ptr<Statement> parseImportStatement();
    std::unique_ptr<Statement> parseStructDeclaration();
    
public:
    explicit Parser(std::vector<Token> inputTokens) {
        tokens.reserve(inputTokens.size());
        for (auto& token : inputTokens) {
            if (token.type != TokenType::NEWLINE) {
                tokens.push_back(std::move(token));
            }
        }
    }
    
    std::vector<std::unique_ptr<Statement>> parse();
};