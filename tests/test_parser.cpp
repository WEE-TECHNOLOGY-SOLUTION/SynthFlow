#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testVariableDeclaration() {
    std::string source = "let x = 42;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    // Check that it's a variable declaration
    auto varDecl = dynamic_cast<VariableDeclaration*>(statements[0].get());
    assert(varDecl != nullptr);
    
    // Check the variable name
    assert(varDecl->name == "x");
    
    // Check the initializer is an integer literal with value 42
    auto initializer = dynamic_cast<IntegerLiteral*>(varDecl->initializer.get());
    assert(initializer != nullptr);
    assert(initializer->value == 42);
    
    std::cout << "Variable declaration test passed!" << std::endl;
}

void testFunctionDeclaration() {
    std::string source = "fn add(a, b) { return a + b; }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    // Check that it's a function declaration
    auto funcDecl = dynamic_cast<FunctionDeclaration*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    // Check the function name
    assert(funcDecl->name == "add");
    
    // Check parameters
    assert(funcDecl->parameters.size() == 2);
    assert(funcDecl->parameters[0] == "a");
    assert(funcDecl->parameters[1] == "b");
    
    std::cout << "Function declaration test passed!" << std::endl;
}

void testExpressionParsing() {
    std::string source = "let result = 10 + 20 * 3;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    // Check that it's a variable declaration
    auto varDecl = dynamic_cast<VariableDeclaration*>(statements[0].get());
    assert(varDecl != nullptr);
    
    // Check the expression is a binary operation
    // Check the expression is a binary operation
    auto binaryOp = dynamic_cast<BinaryExpression*>(varDecl->initializer.get());
    assert(binaryOp != nullptr);
    assert(binaryOp->op == "+");
    
    std::cout << "Expression parsing test passed!" << std::endl;
}

int main() {
    try {
        testVariableDeclaration();
        testFunctionDeclaration();
        testExpressionParsing();
        std::cout << "All parser tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}