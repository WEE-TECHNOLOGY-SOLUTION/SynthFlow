#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/token.h"
#include "../include/ast.h"
#include <stdexcept>
#include <memory>
#include <string>

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
    auto expr = parseLogicalOr();  // Changed from parseEquality to parseLogicalOr
    
    // Handle regular assignment
    if (match(TokenType::ASSIGN)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpression>(std::move(expr), std::move(right));
    }
    
    // Handle compound assignment +=, -=, *=, /=
    if (match(TokenType::PLUS_EQ)) {
        auto right = parseAssignment();
        return std::make_unique<CompoundAssignment>(std::move(expr), "+=", std::move(right));
    }
    if (match(TokenType::MINUS_EQ)) {
        auto right = parseAssignment();
        return std::make_unique<CompoundAssignment>(std::move(expr), "-=", std::move(right));
    }
    if (match(TokenType::STAR_EQ)) {
        auto right = parseAssignment();
        return std::make_unique<CompoundAssignment>(std::move(expr), "*=", std::move(right));
    }
    if (match(TokenType::SLASH_EQ)) {
        auto right = parseAssignment();
        return std::make_unique<CompoundAssignment>(std::move(expr), "/=", std::move(right));
    }
    
    // Handle postfix ++ and --
    if (match(TokenType::PLUS_PLUS)) {
        return std::make_unique<UpdateExpression>(std::move(expr), "++", false);
    }
    if (match(TokenType::MINUS_MINUS)) {
        return std::make_unique<UpdateExpression>(std::move(expr), "--", false);
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

// Parse logical OR (||) - lowest precedence of logical operators
std::unique_ptr<Expression> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::OR)) {
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), "||", std::move(right));
    }
    
    return expr;
}

// Parse logical AND (&&) - higher precedence than OR
std::unique_ptr<Expression> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::AND)) {
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), "&&", std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(TokenType::EQ) || match(TokenType::NE)) {
        Token op = tokens[current - 1];
        auto right = parseComparison();
        std::string opStr = (op.type == TokenType::EQ) ? "==" : "!=";
        expr = std::make_unique<BinaryExpression>(std::move(expr), opStr, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (match(TokenType::LT) || match(TokenType::GT) || 
           match(TokenType::LE) || match(TokenType::GE)) {
        Token op = tokens[current - 1];
        auto right = parseTerm();
        std::string opStr;
        switch (op.type) {
            case TokenType::LT: opStr = "<"; break;
            case TokenType::GT: opStr = ">"; break;
            case TokenType::LE: opStr = "<="; break;
            case TokenType::GE: opStr = ">="; break;
            default: opStr = "<"; break;
        }
        expr = std::make_unique<BinaryExpression>(std::move(expr), opStr, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        std::string opStr = (op.type == TokenType::PLUS) ? "+" : "-";
        expr = std::make_unique<BinaryExpression>(std::move(expr), opStr, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        Token op = tokens[current - 1];
        auto right = parseUnary();
        std::string opStr;
        switch (op.type) {
            case TokenType::STAR: opStr = "*"; break;
            case TokenType::SLASH: opStr = "/"; break;
            case TokenType::PERCENT: opStr = "%"; break;
            default: opStr = "*"; break;
        }
        expr = std::make_unique<BinaryExpression>(std::move(expr), opStr, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::MINUS) || match(TokenType::NOT)) {
        Token op = tokens[current - 1];
        auto right = parseUnary();
        if (op.type == TokenType::MINUS) {
            // Unary minus -> treat as 0 - right? Or UnaryExpression?
            // code_generator/semantic/ast have UnaryExpression with 'op' string.
            // Let's use UnaryExpression!
            return std::make_unique<UnaryExpression>("-", std::move(right));
        }
        if (op.type == TokenType::NOT) {
             return std::make_unique<UnaryExpression>("!", std::move(right));
        }
    }
    
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parseArrayLiteral() {
    // Note: '[' was already consumed by match(LBRACKET) in parsePrimary
    
    auto array = std::make_unique<ArrayLiteral>();
    
    // Handle empty array
    if (match(TokenType::RBRACKET)) {
        return array;
    }
    
    // Parse elements
    do {
        auto element = parseExpression();
        array->elements.push_back(std::move(element)); // elements is vector in ArrayLiteral
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
    
    return std::make_unique<ArrayIndexExpression>(std::move(array), std::move(index));
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
    
    // Handle interpolated strings: "Hello, ${name}!"
    if (match(TokenType::INTERPOLATED_STRING)) {
        std::string value = std::get<std::string>(tokens[current - 1].value);
        std::vector<StringPart> parts;
        
        size_t pos = 0;
        while (pos < value.length()) {
            size_t dollarPos = value.find("${", pos);
            
            if (dollarPos == std::string::npos) {
                // No more interpolations, add rest as text
                if (pos < value.length()) {
                    parts.push_back(StringPart(value.substr(pos)));
                }
                break;
            }
            
            // Add text before ${
            if (dollarPos > pos) {
                parts.push_back(StringPart(value.substr(pos, dollarPos - pos)));
            }
            
            // Find matching }
            size_t braceStart = dollarPos + 2;
            size_t braceEnd = value.find("}", braceStart);
            
            if (braceEnd == std::string::npos) {
                throw std::runtime_error("Unclosed interpolation in string");
            }
            
            // Extract expression string and parse it
            std::string exprStr = value.substr(braceStart, braceEnd - braceStart);
            
            // Create a mini-lexer and parser for the expression
            Lexer exprLexer(exprStr);
            auto exprTokens = exprLexer.tokenize();
            Parser exprParser(exprTokens);
            auto expr = exprParser.parseExpression();
            
            parts.push_back(StringPart(std::move(expr)));
            
            pos = braceEnd + 1;
        }
        
        return std::make_unique<InterpolatedString>(std::move(parts));
    }
    
    if (match(TokenType::BOOLEAN)) {
        bool value = std::get<bool>(tokens[current - 1].value);
        return std::make_unique<BooleanLiteral>(value);
    }
    
    if (match(TokenType::LBRACKET)) {
        return parseArrayLiteral();
    }
    
    // SADK: Map literal { key: value }
    // Disambiguate from block statement by checking for key: pattern
    if (peek().type == TokenType::LBRACE) {
        // Look ahead to determine if this is a map literal or block
        // Map: { key: value } or { "key": value }
        // Block: { statement; }
        size_t savedPos = current;
        advance(); // consume '{'
        
        bool isMap = false;
        // Empty braces is an empty map
        if (peek().type == TokenType::RBRACE) {
            isMap = true;
        }
        // Check for identifier : pattern (map) vs statement (block)
        else if (peek().type == TokenType::STRING || 
                 (peek().type == TokenType::IDENTIFIER && peek(1).type == TokenType::COLON)) {
            isMap = true;
        }
        
        current = savedPos; // restore position
        
        if (isMap) {
            return parseMapLiteral();
        }
        // Otherwise let it fall through to block (handled by caller)
    }
    
    // SADK: self keyword for struct methods
    if (match(TokenType::KW_SELF)) {
        return std::make_unique<SelfExpression>();
    }
    
    // Null literal (for null safety)
    if (match(TokenType::KW_NULL)) {
        return std::make_unique<NullLiteral>();
    }
    
    // Match expression: match x { ... }
    if (match(TokenType::KW_MATCH)) {
        auto subject = parseUnary();  // Parse subject without recursing into full expression
        
        if (!match(TokenType::LBRACE)) {
            throw std::runtime_error("Expected '{' after match expression");
        }
        
        std::vector<MatchCase> cases;
        
        while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
            while (peek().type == TokenType::NEWLINE) advance();
            
            // Parse pattern (or _ for default)
            std::unique_ptr<Expression> pattern = nullptr;
            if (peek().type == TokenType::IDENTIFIER && peek().lexeme == "_") {
                advance(); // consume _
            } else {
                pattern = parsePrimary();  // Parse simple pattern only
            }
            
            if (!match(TokenType::FAT_ARROW)) {
                throw std::runtime_error("Expected '=>' in match case");
            }
            
            auto result = parsePrimary();  // Parse result expression
            
            MatchCase mc;
            mc.pattern = std::move(pattern);
            mc.result = std::move(result);
            cases.push_back(std::move(mc));
            
            // Skip comma and newlines between cases
            match(TokenType::COMMA);
            while (peek().type == TokenType::NEWLINE) advance();
        }
        
        if (!match(TokenType::RBRACE)) {
            throw std::runtime_error("Expected '}' after match cases");
        }
        
        return std::make_unique<MatchExpression>(std::move(subject), std::move(cases));
    }

    // Handle type keywords as callable functions: int(x), float(x), string(x), etc.
    if (peek().type == TokenType::KW_INT || peek().type == TokenType::KW_FLOAT ||
        peek().type == TokenType::KW_STRING || peek().type == TokenType::KW_BOOL ||
        peek().type == TokenType::KW_ARRAY || peek().type == TokenType::KW_MAP) {
        std::string typeName;
        if (match(TokenType::KW_INT)) typeName = "int";
        else if (match(TokenType::KW_FLOAT)) typeName = "float";
        else if (match(TokenType::KW_STRING)) typeName = "string";
        else if (match(TokenType::KW_BOOL)) typeName = "bool";
        else if (match(TokenType::KW_ARRAY)) typeName = "array";
        else if (match(TokenType::KW_MAP)) typeName = "map";
        
        // If followed by '(' then it's a conversion call
        if (peek().type == TokenType::LPAREN) {
            advance(); // consume '('
            std::vector<std::unique_ptr<Expression>> arguments;
            
            if (peek().type != TokenType::RPAREN) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            
            if (!match(TokenType::RPAREN)) {
                throw std::runtime_error("Expected ')' after arguments");
            }
            
            auto callExpr = std::make_unique<CallExpression>(typeName, std::move(arguments));
            return parseCallOrMemberExpression(std::move(callExpr));
        }
        // Otherwise fall through to error (type used alone)
        throw std::runtime_error("Unexpected type keyword '" + typeName + "' - use as call: " + typeName + "(value)");
    }

    if (match(TokenType::IDENTIFIER)) {
        std::string name = tokens[current - 1].lexeme;
        
        // Check if this is a function call
        if (peek().type == TokenType::LPAREN) {
            advance(); // consume '('
            std::vector<std::unique_ptr<Expression>> arguments;
            
            // Parse arguments
            if (peek().type != TokenType::RPAREN) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            
            if (!match(TokenType::RPAREN)) {
                throw std::runtime_error("Expected ')' after function arguments");
            }
            
            auto callExpr = std::make_unique<CallExpression>(name, std::move(arguments));
            // SADK: Check for chained member access: func().field
            return parseCallOrMemberExpression(std::move(callExpr));
        }
        
        auto identifier = std::make_unique<Identifier>(name);
        
        // SADK: Check for member access: obj.field, obj.method()
        if (peek().type == TokenType::DOT) {
            return parseCallOrMemberExpression(std::move(identifier));
        }
        
        // Check if this is an array indexing operation
        if (peek().type == TokenType::LBRACKET) {
            return parseIndexExpression(std::move(identifier));
        }
        
        return identifier;
    }
    
    // Lambda or grouped expression: (x) => x * 2 or (expr)
    if (match(TokenType::LPAREN)) {
        // Could be lambda parameters or grouped expression
        // Check for lambda: (params) =>
        size_t savedPos = current;
        std::vector<std::string> params;
        bool isLambda = false;
        
        // Try to parse as lambda parameters
        if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::RPAREN ||
            peek().type == TokenType::DOT) {  // For variadic ...args
            // Parse potential parameters
            if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::DOT) {
                do {
                    // Check for variadic: ...args
                    if (peek().type == TokenType::DOT && peek(1).type == TokenType::DOT && peek(2).type == TokenType::DOT) {
                        advance(); advance(); advance(); // consume ...
                        if (peek().type == TokenType::IDENTIFIER) {
                            params.push_back("..." + advance().lexeme);
                        }
                        break; // Variadic must be last
                    }
                    if (peek().type != TokenType::IDENTIFIER) break;
                    params.push_back(advance().lexeme);
                    
                    // Skip optional type annotation in lambda: (x: int) => ...
                    if (match(TokenType::COLON)) {
                        // Accept type and skip
                        if (match(TokenType::KW_INT) || match(TokenType::KW_FLOAT) ||
                            match(TokenType::KW_STRING) || match(TokenType::KW_BOOL) ||
                            match(TokenType::KW_ARRAY) || match(TokenType::KW_MAP) ||
                            match(TokenType::IDENTIFIER)) {
                            match(TokenType::QUESTION); // Optional nullable
                        }
                    }
                } while (match(TokenType::COMMA));
            }
            
            if (match(TokenType::RPAREN)) {
                if (peek().type == TokenType::FAT_ARROW) {
                    isLambda = true;
                    advance(); // consume =>
                    
                    // Parse body - single expression or block
                    if (peek().type == TokenType::LBRACE) {
                        auto block = parseBlockStatement();
                        // Cast to BlockStatement
                        auto blockStmt = std::unique_ptr<BlockStatement>(
                            static_cast<BlockStatement*>(block.release()));
                        return std::make_unique<LambdaExpression>(std::move(params), std::move(blockStmt));
                    } else {
                        auto body = parseExpression();
                        return std::make_unique<LambdaExpression>(std::move(params), std::move(body));
                    }
                }
            }
        }
        
        // Not a lambda, backtrack and parse as grouped expression
        current = savedPos;
        auto expr = parseExpression();
        if (!match(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' after expression");
        }
        return expr;
    }
    
    throw std::runtime_error("Unexpected token in primary expression: " + peek().lexeme);
}

std::unique_ptr<Statement> Parser::parseStatement() {
    // SADK: import statement
    if (peek().type == TokenType::KW_IMPORT) {
        return parseImportStatement();
    }
    
    // SADK: struct declaration
    if (peek().type == TokenType::KW_STRUCT) {
        return parseStructDeclaration();
    }
    
    if (peek().type == TokenType::KW_LET) {
        return parseVariableDeclaration();
    }
    
    // const declaration (immutable variable)
    if (peek().type == TokenType::KW_CONST) {
        return parseConstDeclaration();
    }
    
    // try/catch statement
    if (peek().type == TokenType::KW_TRY) {
        return parseTryStatement();
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
    
    if (peek().type == TokenType::LBRACE) {
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
    std::string typeName = "";
    bool isNullable = false;
    
    // Optional type annotation: let x: int = 10
    if (match(TokenType::COLON)) {
        // Parse type name
        if (match(TokenType::KW_INT)) typeName = "int";
        else if (match(TokenType::KW_FLOAT)) typeName = "float";
        else if (match(TokenType::KW_STRING)) typeName = "string";
        else if (match(TokenType::KW_BOOL)) typeName = "bool";
        else if (match(TokenType::KW_ARRAY)) typeName = "array";
        else if (match(TokenType::IDENTIFIER)) typeName = tokens[current - 1].lexeme;
        else throw std::runtime_error("Expected type after ':'");
        
        // Check for nullable type (?)
        if (match(TokenType::QUESTION)) {
            isNullable = true;
        }
    }
    
    if (!match(TokenType::ASSIGN)) {
        throw std::runtime_error("Expected '=' after identifier in variable declaration");
    }
    
    auto initializer = parseExpression();
    match(TokenType::SEMICOLON); // Optional semicolon
    
    return std::make_unique<VariableDeclaration>(name, std::move(initializer), false, typeName, isNullable);
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
            // Check for variadic: ...args
            if (peek().type == TokenType::DOT && peek(1).type == TokenType::DOT && peek(2).type == TokenType::DOT) {
                advance(); advance(); advance(); // consume ...
                if (match(TokenType::IDENTIFIER)) {
                    parameters.push_back("..." + tokens[current - 1].lexeme);
                }
                break; // Variadic must be last
            }
            
            if (!match(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected parameter name");
            }
            parameters.push_back(tokens[current - 1].lexeme);
            
            // Skip optional type annotation: param: type
            if (match(TokenType::COLON)) {
                // Accept any type keyword or identifier as type annotation
                if (match(TokenType::KW_INT) || match(TokenType::KW_FLOAT) ||
                    match(TokenType::KW_STRING) || match(TokenType::KW_BOOL) ||
                    match(TokenType::KW_ARRAY) || match(TokenType::KW_MAP) ||
                    match(TokenType::IDENTIFIER)) {
                    // Type annotation consumed, continue
                    // Also check for nullable type (?)
                    match(TokenType::QUESTION);
                } else {
                    throw std::runtime_error("Expected type after ':'");
                }
            }
        } while (match(TokenType::COMMA));
        
        if (!match(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' after parameters");
        }
    }
    
    // Skip optional return type annotation: -> type
    if (match(TokenType::ARROW)) {
        // Accept any type keyword or identifier as return type
        if (match(TokenType::KW_INT) || match(TokenType::KW_FLOAT) ||
            match(TokenType::KW_STRING) || match(TokenType::KW_BOOL) ||
            match(TokenType::KW_ARRAY) || match(TokenType::KW_MAP) ||
            match(TokenType::IDENTIFIER)) {
            // Return type annotation consumed
            match(TokenType::QUESTION); // Optional nullable
        } else {
            throw std::runtime_error("Expected return type after '->'");
        }
    }
    
    auto block = parseBlockStatement();
    BlockStatement* rawBlock = static_cast<BlockStatement*>(block.release());
    return std::make_unique<FunctionDeclaration>(name, parameters, std::unique_ptr<BlockStatement>(rawBlock));
}

std::unique_ptr<Statement> Parser::parseBlockStatement() {
    if (!match(TokenType::LBRACE)) {
        throw std::runtime_error("Expected '{' at start of block");
    }
    
    auto block = std::make_unique<BlockStatement>();
    
    while (!isAtEnd() && peek().type != TokenType::RBRACE) {
        auto stmt = parseStatement();
        block->statements.push_back(std::move(stmt)); // statements is vector in BlockStatement
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
    
    // thenBranch must be BlockStatement? 
    // ast.h: std::unique_ptr<BlockStatement> thenBranch
    // old parser used parseStatement() which returns Statement.
    // SynthFlow syntax: if (cond) { ... }
    // So it expects a Block.
    // But old parser code (Step 211) Line 342: auto thenBranch = parseStatement();
    // And IfStatement ctor expects BlockStatement.
    // So old code was definitely broken or I'm misreading ast.h?
    // ast.h (Step 43) Line 209: std::unique_ptr<BlockStatement> thenB
    // YES. It expects BlockStatement.
    // So 'if (cond) stmt;' is NOT supported by AST?
    // If AST enforces BlockStatement, then parser must ensure it parses a block or wraps stmt in block.
    
    // I will try to parseBlockStatement() directly.
    // But what if user writes `if (x) return;`?
    // If language enforces `{}` then `parseBlockStatement` is correct.
    // Parse then branch using parseStatement
    auto thenStmt = parseStatement();
    BlockStatement* thenBlock = dynamic_cast<BlockStatement*>(thenStmt.get());
    std::unique_ptr<BlockStatement> realThen;
    if (thenBlock) {
        thenStmt.release(); // ownership transferred
        realThen.reset(thenBlock);
    } else {
        // Wrap single statement in a block
        realThen = std::make_unique<BlockStatement>();
        realThen->statements.push_back(std::move(thenStmt));
    }

    std::unique_ptr<BlockStatement> realElse = nullptr;
    if (match(TokenType::KW_ELSE)) {
        auto elseStmt = parseStatement();
        BlockStatement* elseBlock = dynamic_cast<BlockStatement*>(elseStmt.get());
        if (elseBlock) {
            elseStmt.release();
            realElse.reset(elseBlock);
        } else {
            realElse = std::make_unique<BlockStatement>();
            realElse->statements.push_back(std::move(elseStmt));
        }
    }
    
    return std::make_unique<IfStatement>(std::move(condition), std::move(realThen), std::move(realElse));
}

std::unique_ptr<Statement> Parser::parseWhileStatement() {
    advance(); // consume 'while'
    if (!match(TokenType::LPAREN)) throw std::runtime_error("Expected '('");
    auto condition = parseExpression();
    if (!match(TokenType::RPAREN)) throw std::runtime_error("Expected ')'");
    
    auto bodyRaw = parseStatement();
    BlockStatement* bb = dynamic_cast<BlockStatement*>(bodyRaw.get());
    std::unique_ptr<BlockStatement> realBody;
    if (bb) {
        bodyRaw.release();
        realBody.reset(bb);
    } else {
        realBody = std::make_unique<BlockStatement>();
        realBody->statements.push_back(std::move(bodyRaw));
    }
    
    return std::make_unique<WhileStatement>(std::move(condition), std::move(realBody));
}

std::unique_ptr<Statement> Parser::parseBreakStatement() {
    advance();
    match(TokenType::SEMICOLON);
    return std::make_unique<BreakStatement>();
}

std::unique_ptr<Statement> Parser::parseContinueStatement() {
    advance();
    match(TokenType::SEMICOLON);
    return std::make_unique<ContinueStatement>();
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    advance();
    std::unique_ptr<Expression> val = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        val = parseExpression();
    }
    match(TokenType::SEMICOLON);
    return std::make_unique<ReturnStatement>(std::move(val));
}

std::unique_ptr<Statement> Parser::parseForStatement() {
    advance();
    if (!match(TokenType::LPAREN)) throw std::runtime_error("Expected '('");
    
    std::unique_ptr<Statement> init = nullptr;
    if (peek().type == TokenType::KW_LET) {
        init = parseVariableDeclaration();
    } else if (peek().type != TokenType::SEMICOLON) {
        auto expr = parseExpression();
        match(TokenType::SEMICOLON);
        init = std::make_unique<ExpressionStatement>(std::move(expr));
    } else {
        match(TokenType::SEMICOLON);
    }
    
    std::unique_ptr<Expression> cond = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        cond = parseExpression();
    }
    match(TokenType::SEMICOLON);
    
    std::unique_ptr<Expression> incr = nullptr;
    if (peek().type != TokenType::RPAREN) {
        incr = parseExpression();
    }
    
    if (!match(TokenType::RPAREN)) throw std::runtime_error("Expected ')'");
    
    auto bodyRaw = parseStatement();
    BlockStatement* bb = dynamic_cast<BlockStatement*>(bodyRaw.get());
    std::unique_ptr<BlockStatement> realBody;
    if (bb) {
        bodyRaw.release();
        realBody.reset(bb);
    } else {
        realBody = std::make_unique<BlockStatement>();
        realBody->statements.push_back(std::move(bodyRaw));
    }
    
    return std::make_unique<ForStatement>(std::move(init), std::move(cond), std::move(incr), std::move(realBody));
}

std::unique_ptr<Statement> Parser::parseConstDeclaration() {
    advance(); // consume 'const'
    
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected identifier after 'const'");
    }
    
    std::string name = tokens[current - 1].lexeme;
    std::string typeName = "";
    bool isNullable = false;
    
    // Optional type annotation: const PI: float = 3.14
    if (match(TokenType::COLON)) {
        if (match(TokenType::KW_INT)) typeName = "int";
        else if (match(TokenType::KW_FLOAT)) typeName = "float";
        else if (match(TokenType::KW_STRING)) typeName = "string";
        else if (match(TokenType::KW_BOOL)) typeName = "bool";
        else if (match(TokenType::KW_ARRAY)) typeName = "array";
        else if (match(TokenType::IDENTIFIER)) typeName = tokens[current - 1].lexeme;
        else throw std::runtime_error("Expected type after ':'");
        
        if (match(TokenType::QUESTION)) {
            isNullable = true;
        }
    }
    
    if (!match(TokenType::ASSIGN)) {
        throw std::runtime_error("Expected '=' in const declaration");
    }
    
    auto initializer = parseExpression();
    match(TokenType::SEMICOLON);
    
    return std::make_unique<VariableDeclaration>(name, std::move(initializer), true, typeName, isNullable);
}

std::unique_ptr<Statement> Parser::parseTryStatement() {
    advance(); // consume 'try'
    
    // Parse try block
    auto tryBlock = parseBlockStatement();
    BlockStatement* tbb = dynamic_cast<BlockStatement*>(tryBlock.get());
    std::unique_ptr<BlockStatement> realTry;
    if (tbb) {
        tryBlock.release();
        realTry.reset(tbb);
    } else {
        realTry = std::make_unique<BlockStatement>();
        realTry->statements.push_back(std::move(tryBlock));
    }
    
    // Expect 'catch'
    if (!match(TokenType::KW_CATCH)) {
        throw std::runtime_error("Expected 'catch' after try block");
    }
    
    // Parse error variable
    if (!match(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'catch'");
    }
    
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected error variable name in catch");
    }
    std::string errorVar = tokens[current - 1].lexeme;
    
    if (!match(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after error variable");
    }
    
    // Parse catch block
    auto catchBlock = parseBlockStatement();
    BlockStatement* cbb = dynamic_cast<BlockStatement*>(catchBlock.get());
    std::unique_ptr<BlockStatement> realCatch;
    if (cbb) {
        catchBlock.release();
        realCatch.reset(cbb);
    } else {
        realCatch = std::make_unique<BlockStatement>();
        realCatch->statements.push_back(std::move(catchBlock));
    }
    
    return std::make_unique<TryStatement>(std::move(realTry), errorVar, std::move(realCatch));
}

// ========================================
// SADK (Agent Development Kit) Parsing
// ========================================

std::unique_ptr<Expression> Parser::parseMapLiteral() {
    advance(); // consume '{'
    
    auto map = std::make_unique<MapLiteral>();
    
    // Handle empty map
    if (match(TokenType::RBRACE)) {
        return map;
    }
    
    // Parse key-value pairs
    do {
        // Skip newlines inside map
        while (peek().type == TokenType::NEWLINE) advance();
        
        if (peek().type == TokenType::RBRACE) break;
        
        // Parse key (string or identifier)
        std::unique_ptr<Expression> key;
        if (peek().type == TokenType::STRING) {
            advance();
            std::string keyStr = std::get<std::string>(tokens[current - 1].value);
            key = std::make_unique<StringLiteral>(keyStr);
        } else if (peek().type == TokenType::IDENTIFIER) {
            advance();
            std::string keyStr = tokens[current - 1].lexeme;
            key = std::make_unique<StringLiteral>(keyStr);  // Convert identifier to string key
        } else {
            throw std::runtime_error("Expected string or identifier as map key");
        }
        
        // Expect colon
        if (!match(TokenType::COLON)) {
            throw std::runtime_error("Expected ':' after map key");
        }
        
        // Parse value
        auto value = parseExpression();
        
        map->addEntry(std::move(key), std::move(value));
        
        // Skip newlines
        while (peek().type == TokenType::NEWLINE) advance();
        
    } while (match(TokenType::COMMA));
    
    // Skip newlines before closing brace
    while (peek().type == TokenType::NEWLINE) advance();
    
    if (!match(TokenType::RBRACE)) {
        throw std::runtime_error("Expected '}' at end of map literal");
    }
    
    return map;
}

std::unique_ptr<Expression> Parser::parseCallOrMemberExpression(std::unique_ptr<Expression> expr) {
    while (true) {
        if (match(TokenType::DOT)) {
            // Member access: obj.field
            if (!match(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected identifier after '.'");
            }
            std::string member = tokens[current - 1].lexeme;
            
            // Check if it's a method call: obj.method()
            if (peek().type == TokenType::LPAREN) {
                advance(); // consume '('
                std::vector<std::unique_ptr<Expression>> arguments;
                
                if (peek().type != TokenType::RPAREN) {
                    do {
                        arguments.push_back(parseExpression());
                    } while (match(TokenType::COMMA));
                }
                
                if (!match(TokenType::RPAREN)) {
                    throw std::runtime_error("Expected ')' after method arguments");
                }
                
                // Create member access first, then wrap in a call
                auto memberExpr = std::make_unique<MemberExpression>(std::move(expr), member);
                // For now, method calls are represented as CallExpression on member name
                // A proper implementation would need a MethodCallExpression or CallExpression with callee as Expression
                // Simplified: we'll store the method name and handle in interpreter
                expr = std::make_unique<MemberExpression>(std::move(memberExpr), "<call>");  // Placeholder
            } else {
                expr = std::make_unique<MemberExpression>(std::move(expr), member);
            }
        } else if (peek().type == TokenType::LBRACKET) {
            // Computed member access: obj["field"]
            advance(); // consume '['
            auto index = parseExpression();
            
            if (!match(TokenType::RBRACKET)) {
                throw std::runtime_error("Expected ']' after index");
            }
            
            // Check if the object is an array or map
            auto indexExpr = std::make_unique<ArrayIndexExpression>(std::move(expr), std::move(index));
            expr = std::move(indexExpr);
        } else if (peek().type == TokenType::LPAREN) {
            // Function call on expression result
            advance(); // consume '('
            std::vector<std::unique_ptr<Expression>> arguments;
            
            if (peek().type != TokenType::RPAREN) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            
            if (!match(TokenType::RPAREN)) {
                throw std::runtime_error("Expected ')' after arguments");
            }
            
            // For identifier expressions, create a proper CallExpression
            if (auto* ident = dynamic_cast<Identifier*>(expr.get())) {
                std::string name = ident->name;
                expr = std::make_unique<CallExpression>(name, std::move(arguments));
            } else {
                // General callable - not yet supported in full
                throw std::runtime_error("Callable expressions not fully supported yet");
            }
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Statement> Parser::parseImportStatement() {
    advance(); // consume 'import'
    
    // Parse module name
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected module name after 'import'");
    }
    std::string moduleName = tokens[current - 1].lexeme;
    
    auto importStmt = std::make_unique<ImportStatement>(moduleName);
    
    // Check for 'from' clause: import io from "path"
    if (match(TokenType::KW_FROM)) {
        if (!match(TokenType::STRING)) {
            throw std::runtime_error("Expected string path after 'from'");
        }
        importStmt->modulePath = std::get<std::string>(tokens[current - 1].value);
    }
    
    // Check for 'as' clause: import io as fileIO
    if (match(TokenType::KW_AS)) {
        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected alias after 'as'");
        }
        importStmt->alias = tokens[current - 1].lexeme;
    }
    
    match(TokenType::SEMICOLON);
    return importStmt;
}

std::unique_ptr<Statement> Parser::parseStructDeclaration() {
    advance(); // consume 'struct'
    
    // Parse struct name
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected struct name after 'struct'");
    }
    std::string structName = tokens[current - 1].lexeme;
    
    auto structDecl = std::make_unique<StructDeclaration>(structName);
    
    // Check for 'extends' clause
    if (match(TokenType::KW_EXTENDS)) {
        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected parent struct name after 'extends'");
        }
        structDecl->parentStruct = tokens[current - 1].lexeme;
    }
    
    // Parse struct body
    if (!match(TokenType::LBRACE)) {
        throw std::runtime_error("Expected '{' after struct name");
    }
    
    while (!isAtEnd() && peek().type != TokenType::RBRACE) {
        // Skip newlines
        while (peek().type == TokenType::NEWLINE) advance();
        
        if (peek().type == TokenType::RBRACE) break;
        
        // Check for method declaration (fn keyword)
        if (peek().type == TokenType::KW_FN) {
            auto method = parseFunctionDeclaration();
            structDecl->addMethod(std::unique_ptr<FunctionDeclaration>(
                static_cast<FunctionDeclaration*>(method.release())));
        }
        // Otherwise parse as field: name: type
        else if (peek().type == TokenType::IDENTIFIER) {
            advance();
            std::string fieldName = tokens[current - 1].lexeme;
            
            if (!match(TokenType::COLON)) {
                throw std::runtime_error("Expected ':' after field name");
            }
            
            // Parse type
            std::string typeName;
            if (match(TokenType::KW_INT)) typeName = "int";
            else if (match(TokenType::KW_FLOAT)) typeName = "float";
            else if (match(TokenType::KW_STRING)) typeName = "string";
            else if (match(TokenType::KW_BOOL)) typeName = "bool";
            else if (match(TokenType::KW_ARRAY)) typeName = "array";
            else if (match(TokenType::KW_MAP)) typeName = "map";
            else if (match(TokenType::IDENTIFIER)) typeName = tokens[current - 1].lexeme;
            else throw std::runtime_error("Expected type for field");
            
            structDecl->addField(fieldName, typeName);
            
            // Optional comma
            match(TokenType::COMMA);
        } else {
            throw std::runtime_error("Expected field or method in struct");
        }
        
        // Skip newlines
        while (peek().type == TokenType::NEWLINE) advance();
    }
    
    if (!match(TokenType::RBRACE)) {
        throw std::runtime_error("Expected '}' at end of struct");
    }
    
    return structDecl;
}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return statements;
}