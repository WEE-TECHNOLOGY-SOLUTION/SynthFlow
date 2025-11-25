#include "../include/parser.h"
#include "../include/token.h"
#include "../include/ast.h"
#include <stdexcept>
#include <memory>

Token& Parser::peek(int offset) {
    size_t index = current + offset;
    if (index >= tokens.size()) {
        return tokens[tokens.size() - 1]; // Return EOF token
    }
    return tokens[index];
}

Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

std::unique_ptr<Expression> Parser::parseAssignment() {
    auto expr = parseEquality();
    
    if (match(TokenType::ASSIGN)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpression>(std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(TokenType::EQ) || match(TokenType::NE)) {
        Token op = tokens[current - 1];
        auto right = parseComparison();
        BinaryOp::OpType opType = (op.type == TokenType::EQ) ? BinaryOp::EQ : BinaryOp::NE;
        expr = std::make_unique<BinaryOp>(std::move(expr), opType, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (match(TokenType::LT) || match(TokenType::GT) || 
           match(TokenType::LE) || match(TokenType::GE)) {
        Token op = tokens[current - 1];
        auto right = parseTerm();
        BinaryOp::OpType opType;
        switch (op.type) {
            case TokenType::LT: opType = BinaryOp::LT; break;
            case TokenType::GT: opType = BinaryOp::GT; break;
            case TokenType::LE: opType = BinaryOp::LE; break;
            case TokenType::GE: opType = BinaryOp::GE; break;
            default: opType = BinaryOp::LT; break; // Should never happen
        }
        expr = std::make_unique<BinaryOp>(std::move(expr), opType, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        BinaryOp::OpType opType = (op.type == TokenType::PLUS) ? BinaryOp::ADD : BinaryOp::SUB;
        expr = std::make_unique<BinaryOp>(std::move(expr), opType, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        Token op = tokens[current - 1];
        auto right = parseUnary();
        BinaryOp::OpType opType;
        switch (op.type) {
            case TokenType::STAR: opType = BinaryOp::MUL; break;
            case TokenType::SLASH: opType = BinaryOp::DIV; break;
            case TokenType::PERCENT: opType = BinaryOp::MOD; break;
            default: opType = BinaryOp::MUL; break; // Should never happen
        }
        expr = std::make_unique<BinaryOp>(std::move(expr), opType, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::MINUS) || match(TokenType::NOT)) {
        Token op = tokens[current - 1];
        auto right = parseUnary();
        // For simplicity, we'll treat unary minus as a binary operation with 0
        if (op.type == TokenType::MINUS) {
            auto zero = std::make_unique<IntegerLiteral>(0);
            return std::make_unique<BinaryOp>(std::move(zero), BinaryOp::SUB, std::move(right));
        }
        // TODO: Handle NOT operator
    }
    
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parseArrayLiteral() {
    advance(); // consume '['
    
    auto array = std::make_unique<ArrayLiteral>();
    
    // Handle empty array
    if (match(TokenType::RBRACKET)) {
        return array;
    }
    
    // Parse elements
    do {
        auto element = parseExpression();
        array->addElement(std::move(element));
    } while (match(TokenType::COMMA));
    
    if (!match(TokenType::RBRACKET)) {
        throw std::runtime_error("Expected ']' at end of array literal");
    }
    
    return array;
}

std::unique_ptr<Expression> Parser::parseIndexExpression(std::unique_ptr<Expression> array) {
    advance(); // consume '['
    
    auto index = parseExpression();
    
    if (!match(TokenType::RBRACKET)) {
        throw std::runtime_error("Expected ']' after index expression");
    }
    
    return std::make_unique<IndexExpression>(std::move(array), std::move(index));
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::INTEGER)) {
        int64_t value = std::get<int64_t>(tokens[current - 1].value);
        return std::make_unique<IntegerLiteral>(value);
    }
    
    if (match(TokenType::FLOAT)) {
        double value = std::get<double>(tokens[current - 1].value);
        return std::make_unique<FloatLiteral>(value);
    }
    
    if (match(TokenType::STRING)) {
        std::string value = std::get<std::string>(tokens[current - 1].value);
        return std::make_unique<StringLiteral>(value);
    }
    
    if (match(TokenType::BOOLEAN)) {
        bool value = std::get<bool>(tokens[current - 1].value);
        return std::make_unique<BooleanLiteral>(value);
    }
    
    if (match(TokenType::LBRACKET)) {
        return parseArrayLiteral();
    }
    
    if (match(TokenType::IDENTIFIER)) {
        std::string name = tokens[current - 1].lexeme;
        auto identifier = std::make_unique<Identifier>(name);
        
        // Check if this is an array indexing operation
        if (peek().type == TokenType::LBRACKET) {
            return parseIndexExpression(std::move(identifier));
        }
        
        return identifier;
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' after expression");
        }
        return expr;
    }
    
    throw std::runtime_error("Unexpected token in primary expression");
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (peek().type == TokenType::KW_LET) {
        return parseVariableDeclaration();
    }
    
    if (peek().type == TokenType::KW_FN) {
        return parseFunctionDeclaration();
    }
    
    if (peek().type == TokenType::KW_IF) {
        return parseIfStatement();
    }
    
    if (peek().type == TokenType::KW_WHILE) {
        return parseWhileStatement();
    }
    
    if (peek().type == TokenType::KW_FOR) {
        return parseForStatement();
    }
    
    if (peek().type == TokenType::KW_BREAK) {
        return parseBreakStatement();
    }
    
    if (peek().type == TokenType::KW_CONTINUE) {
        return parseContinueStatement();
    }
    
    if (peek().type == TokenType::KW_RETURN) {
        return parseReturnStatement();
    }
    
    if (match(TokenType::LBRACE)) {
        return parseBlockStatement();
    }
    
    return parseExpressionStatement();
}

std::unique_ptr<Statement> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    match(TokenType::SEMICOLON); // Optional semicolon
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    advance(); // consume 'let'
    
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected identifier after 'let'");
    }
    
    std::string name = tokens[current - 1].lexeme;
    
    if (!match(TokenType::ASSIGN)) {
        throw std::runtime_error("Expected '=' after identifier in variable declaration");
    }
    
    auto initializer = parseExpression();
    match(TokenType::SEMICOLON); // Optional semicolon
    
    return std::make_unique<VariableDeclaration>(name, std::move(initializer));
}

std::unique_ptr<Statement> Parser::parseFunctionDeclaration() {
    advance(); // consume 'fn'
    
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected identifier after 'fn'");
    }
    
    std::string name = tokens[current - 1].lexeme;
    
    if (!match(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after function name");
    }
    
    std::vector<std::string> parameters;
    if (!match(TokenType::RPAREN)) {
        do {
            if (!match(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected parameter name");
            }
            parameters.push_back(tokens[current - 1].lexeme);
        } while (match(TokenType::COMMA));
        
        if (!match(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' after parameters");
        }
    }
    
    auto body = parseBlockStatement();
    
    return std::make_unique<FunctionDeclaration>(name, std::move(parameters), std::move(body));
}

std::unique_ptr<Statement> Parser::parseBlockStatement() {
    if (!match(TokenType::LBRACE)) {
        throw std::runtime_error("Expected '{' at start of block");
    }
    
    auto block = std::make_unique<BlockStatement>();
    
    while (!isAtEnd() && peek().type != TokenType::RBRACE) {
        auto stmt = parseStatement();
        block->addStatement(std::move(stmt));
    }
    
    if (!match(TokenType::RBRACE)) {
        throw std::runtime_error("Expected '}' at end of block");
    }
    
    return block;
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    advance(); // consume 'if'
    
    if (!match(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'if'");
    }
    
    auto condition = parseExpression();
    
    if (!match(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after if condition");
    }
    
    auto thenBranch = parseStatement();
    std::unique_ptr<Statement> elseBranch = nullptr;
    
    if (match(TokenType::KW_ELSE)) {
        elseBranch = parseStatement();
    }
    
    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::parseWhileStatement() {
    advance(); // consume 'while'
    
    if (!match(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'while'");
    }
    
    auto condition = parseExpression();
    
    if (!match(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after while condition");
    }
    
    auto body = parseStatement();
    
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::parseBreakStatement() {
    advance(); // consume 'break'
    match(TokenType::SEMICOLON); // Optional semicolon
    return std::make_unique<BreakStatement>();
}

std::unique_ptr<Statement> Parser::parseContinueStatement() {
    advance(); // consume 'continue'
    match(TokenType::SEMICOLON); // Optional semicolon
    return std::make_unique<ContinueStatement>();
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    advance(); // consume 'return'
    
    std::unique_ptr<Expression> returnValue = nullptr;
    if (peek().type != TokenType::SEMICOLON && peek().type != TokenType::NEWLINE) {
        returnValue = parseExpression();
    }
    
    match(TokenType::SEMICOLON); // Optional semicolon
    
    return std::make_unique<ReturnStatement>(std::move(returnValue));
}

std::unique_ptr<Statement> Parser::parseForStatement() {
    advance(); // consume 'for'
    
    if (!match(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'for'");
    }
    
    // Parse initializer (can be a variable declaration or expression)
    std::unique_ptr<Statement> initializer = nullptr;
    if (peek().type == TokenType::KW_LET) {
        initializer = parseVariableDeclaration();
    } else if (peek().type != TokenType::SEMICOLON) {
        auto expr = parseExpression();
        match(TokenType::SEMICOLON); // consume semicolon after expression
        initializer = std::make_unique<ExpressionStatement>(std::move(expr));
    } else {
        match(TokenType::SEMICOLON); // consume empty initializer
    }
    
    // Parse condition (optional)
    std::unique_ptr<Expression> condition = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        condition = parseExpression();
    }
    match(TokenType::SEMICOLON); // consume semicolon after condition
    
    // Parse increment (optional)
    std::unique_ptr<Expression> increment = nullptr;
    if (peek().type != TokenType::RPAREN) {
        increment = parseExpression();
    }
    
    if (!match(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after for clause");
    }
    
    // Parse body
    auto body = parseStatement();
    
    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), 
                                         std::move(increment), std::move(body));
}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    
    return statements;
}