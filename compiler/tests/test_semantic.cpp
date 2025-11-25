#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantic_analyzer.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testValidCode() {
    std::string source = "let x = 42; let y = x + 10;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    std::cout << "Valid code test passed!" << std::endl;
}

void testUndeclaredVariable() {
    std::string source = "let y = x + 10;"; // x is not declared
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        assert(false && "Should have thrown an exception");
    } catch (const std::runtime_error& e) {
        std::cout << "Undeclared variable test passed!" << std::endl;
    }
}

void testVariableRedeclaration() {
    std::string source = "let x = 42; let x = 10;"; // x is declared twice
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        assert(false && "Should have thrown an exception");
    } catch (const std::runtime_error& e) {
        std::cout << "Variable redeclaration test passed!" << std::endl;
    }
}

int main() {
    try {
        testValidCode();
        testUndeclaredVariable();
        testVariableRedeclaration();
        std::cout << "All semantic analyzer tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}