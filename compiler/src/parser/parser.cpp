#include "../include/parser.h"
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
    auto expr = parseEquality();
    
    if (match(TokenType::ASSIGN)) {
        auto right = parseAssignment();
        // Check if expr is a valid assignment target (l-value)
        // For now, we wrap it in AssignmentExpression, but AST has specific structure?
        // ast.h: AssignmentExpression(unique_ptr<Expression> l, unique_ptr<Expression> r)
        // CodeGen expects left to be accepted.
        
        // Wait, what if it's array assignment? `arr[i] = val`
        // ast.h has ArrayAssignmentExpression.
        // But parser logic for `arr[i] = val` usually parses `arr[i]` as IndexExpression first.
        // If we want to support ArrayAssignmentExpression, we might need to cast 'expr' or check its type.
        // However, standard AssignmentExpression handles general assignment. 
        // If CodeGen/Semantic handle AssignmentExpression generically, we are fine.
        // CodeGen::visit(AssignmentExpression) calls left->accept().
        // If left is ArrayIndexExpression, visitor outputs `arr[i]`.
        // Then outputs ` = `. Then right.
        // `arr[i] = val`. This WORKS via standard AssignmentExpression!
        // So ArrayAssignmentExpression in AST might be redundant or for optimization?
        // For now, I will use AssignmentExpression for everything to be safe.
        
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
    advance(); // consume '['
    
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
    
    if (match(TokenType::BOOLEAN)) {
        bool value = std::get<bool>(tokens[current - 1].value);
        return std::make_unique<BooleanLiteral>(value);
    }
    
    if (match(TokenType::LBRACKET)) {
        return parseArrayLiteral();
    }
    
    // Null literal (for null safety)
    if (match(TokenType::KW_NULL)) {
        return std::make_unique<NullLiteral>();
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
            
            return std::make_unique<CallExpression>(name, std::move(arguments));
        }
        
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
    
    throw std::runtime_error("Unexpected token in primary expression: " + peek().lexeme);
}

std::unique_ptr<Statement> Parser::parseStatement() {
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
            if (!match(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected parameter name");
            }
            parameters.push_back(tokens[current - 1].lexeme);
        } while (match(TokenType::COMMA));
        
        if (!match(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' after parameters");
        }
    }
    
    auto block = parseBlockStatement();
    // parseBlockStatement returns unique_ptr<Statement>, but FunctionDeclaration needs unique_ptr<BlockStatement>
    // We need to cast it or change parseBlockStatement to return concrete type?
    // ast.h: FunctionDeclaration(..., unique_ptr<BlockStatement> b)
    // Code in Step 211 had: auto body = parseBlockStatement(); ... std::make_unique<FunctionDeclaration>(..., std::move(body));
    // If parseBlockStatement returned Statement, move would fail?
    // Let's check parseBlockStatement return type in old code.
    // Line 310: std::unique_ptr<Statement> Parser::parseBlockStatement()
    // It constructs BlockStatement but returns Statement.
    // So 'body' was Statement. FunctionDeclaration ctor expects BlockStatement.
    // OLD CODE WAS BROKEN HERE TOO? Or FunctionDeclaration accepted Statement?
    // Check ast.h Step 43.
    // Line 271: FunctionDeclaration(..., std::unique_ptr<BlockStatement> b)
    // So distinct types. unique_ptr<Statement> CANNOT be moved into unique_ptr<BlockStatement>.
    // This implies parseBlockStatement SHOULD return unique_ptr<BlockStatement>!
    // But in header parser.h (Step 241) it returns unique_ptr<Statement>.
    // I should cast it. static_cast? No, dynamic_cast with pointers.
    // Or just change parseBlockStatement signature provided I change header too.
    // Changing header is easy.
    // For now, I'll use static_cast on the raw pointer and release ownership?
    // std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(stmt.release()))
    
    // I will use that cast logic to be safe without changing header right now.
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
    // Old parser (Line 246) called `parseBlockStatement` only if `{`.
    // If strict, `parseBlockStatement`.
    // I'll assume strict `{}` for now to match AST requirements easily.
    
    auto thenStmt = parseStatement();
    // Verify it is a block?
    // If I use the cast trick, I assume it IS a block.
    // If parseStatement returns BlockStatement (wrapped in Statement), cast works.
    // But parseStatement calls parseBlockStatement if `{`.
    
    // CAST TRICK:
    BlockStatement* thenBlock = dynamic_cast<BlockStatement*>(thenStmt.get());
    if (!thenBlock) {
         // Create a wrapper block? Or error?
         // If language allows single statement, wrap it.
         auto wrapper = std::make_unique<BlockStatement>();
         wrapper->statements.push_back(std::move(thenStmt)); // We moved from thenStmt? No.
         // We need to move unique_ptr.
         // Let's just assume we need to cast.
         // For reliability, I will change IfStatement in AST to take Statement*?
         // No, simpler to wrap.
         // I'll write code to wrap provided statement into a BlockStatement if it's not one.
         // BUT wait, `thenBlock` check on `get()` is unsafe if I move `thenStmt` later.
         // `thenStmt` is unique_ptr from parseStatement.
         // I'll create a new BlockStatement, add the stmt.
         // But wait, if it IS a block, we double wrap?
         // Let's just CAST. If it fails (nullptr), we wrap.
    }
    
    // Actually, simplifying: The previous parser code was calling parseStatement().
    // I'll handle the cast.
    std::unique_ptr<BlockStatement> thenBlockPtr;
    // We need to release ownership from thenStmt to cast/wrap.
    // Since unique_ptr doesn't support easy dynamic_cast ownership transfer...
    // I will manually reconstruct.
    
    // Wait, If I rely on `if (match(LBRACE))` logic inside `parseIfStatement`, I can call `parseBlockStatement` directly!
    // But `parseStatement` dispatches.
    
    // Let's enforce `{}` for 'if'.
    // `if (peek().type != LBRACE) error("Expected {")`.
    // This is valid since `ast.h` requires BlockStatement.
    
    // Wait, `parseStatement` calls `parseBlockStatement`.
    // I will call `parseBlockStatement` directly inside `parseIfStatement`.
    // BUT `parseBlockStatement` returns `unique_ptr<Statement>`.
    // I'll cast the result of `parseBlockStatement`.
    
    // Wait, I should change `parseBlockStatement` to return `unique_ptr<BlockStatement>` in header and cpp! 
    // That solves everything.
    // But checking header `parser.h` is hard? No, I viewed it.
    // I'll cast for now.
    
    auto thenRaw = parseStatement();
    // NOTE: This assumes thenRaw IS a BlockStatement. If parseStatement returned ExpressionStatement, dynamic_cast fails.
    // If dynamic_cast fails, we wrap it?
    // Let's implement wrap logic.
    BlockStatement* tb = dynamic_cast<BlockStatement*>(thenRaw.get());
    std::unique_ptr<BlockStatement> realThen;
    if (tb) {
         thenRaw.release(); // validation passed
         realThen.reset(tb);
    } else {
         realThen = std::make_unique<BlockStatement>();
         realThen->statements.push_back(std::move(thenRaw));
    }

    std::unique_ptr<BlockStatement> realElse = nullptr;
    if (match(TokenType::KW_ELSE)) {
        auto elseRaw = parseStatement();
        BlockStatement* eb = dynamic_cast<BlockStatement*>(elseRaw.get());
        if (eb) {
             elseRaw.release(); 
             realElse.reset(eb);
        } else {
             realElse = std::make_unique<BlockStatement>();
             realElse->statements.push_back(std::move(elseRaw));
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

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return statements;
}