#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantic_analyzer.h"
#include "../include/code_generator.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testArrayLiteral() {
    std::string source = "let arr = [1, 2, 3, 4, 5];";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Array literal test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

void testArrayIndexing() {
    std::string source = "let arr = [1, 2, 3]; let x = arr[0];";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Array indexing test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

void testEmptyArray() {
    std::string source = "let arr = [];";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Empty array test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

void testNestedArray() {
    std::string source = "let arr = [[1, 2], [3, 4]]; let x = arr[0][1];";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Nested array test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

void testArrayAssignment() {
    std::string source = "let arr = [1, 2, 3]; arr[0] = 10;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Array assignment test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

int main() {
    try {
        testArrayLiteral();
        testArrayIndexing();
        testEmptyArray();
        testNestedArray();
        testArrayAssignment();
        std::cout << "All array tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}