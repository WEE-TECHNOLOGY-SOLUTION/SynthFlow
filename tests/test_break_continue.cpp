#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantic_analyzer.h"
#include "../include/code_generator.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testBreakInLoop() {
    std::string source = "let x = 0;\nwhile (x < 10) {\n  x = x + 1;\n  if (x == 5) break;\n}";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Break in loop test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

void testContinueInLoop() {
    std::string source = "let x = 0;\nwhile (x < 10) {\n  x = x + 1;\n  if (x == 5) continue;\n  let y = x * 2;\n}";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    // This should not throw an exception
    analyzer.analyze(statements);
    
    CodeGenerator generator;
    std::string generatedCode = generator.generate(statements);
    
    std::cout << "Continue in loop test passed!" << std::endl;
    std::cout << "Generated code:" << std::endl << generatedCode << std::endl;
}

void testBreakOutsideLoop() {
    std::string source = "let x = 0;\nbreak;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        // Should not reach here
        assert(false && "Expected semantic error for break outside loop");
    } catch (const std::runtime_error& e) {
        std::cout << "Break outside loop test passed! Caught expected error: " << e.what() << std::endl;
    }
}

void testContinueOutsideLoop() {
    std::string source = "let x = 0;\ncontinue;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        // Should not reach here
        assert(false && "Expected semantic error for continue outside loop");
    } catch (const std::runtime_error& e) {
        std::cout << "Continue outside loop test passed! Caught expected error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        testBreakInLoop();
        testContinueInLoop();
        testBreakOutsideLoop();
        testContinueOutsideLoop();
        std::cout << "All break/continue tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}