#include "../include/lexer.h"
#include "../include/parser.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testForLoopParsing() {
    std::string source = "for (let i = 0; i < 10; i = i + 1) { let x = i * 2; }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    std::cout << "For loop parsing test passed!" << std::endl;
}

void testForLoopWithBreak() {
    std::string source = "for (let i = 0; i < 10; i = i + 1) { if (i == 5) break; }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    std::cout << "For loop with break parsing test passed!" << std::endl;
}

void testForLoopWithContinue() {
    std::string source = "for (let i = 0; i < 10; i = i + 1) { if (i == 5) continue; let x = i * 2; }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    std::cout << "For loop with continue parsing test passed!" << std::endl;
}

int main() {
    try {
        testForLoopParsing();
        testForLoopWithBreak();
        testForLoopWithContinue();
        std::cout << "All for loop tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}