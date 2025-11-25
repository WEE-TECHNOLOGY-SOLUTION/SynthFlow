#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/code_generator.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testCodeGeneration() {
    std::string source = "let x = 42; let y = x + 10;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Code generation test passed!" << std::endl;
    std::cout << "Generated code:\n" << generatedCode << std::endl;
}

void testFunctionGeneration() {
    std::string source = "fn add(a, b) { return a + b; }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Function generation test passed!" << std::endl;
    std::cout << "Generated code:\n" << generatedCode << std::endl;
}

int main() {
    try {
        testCodeGeneration();
        testFunctionGeneration();
        std::cout << "All code generation tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}