#include "../include/lexer.h"
#include "../include/parser.h"
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>

void testWhileLoopParsing() {
    std::string source = "while (x < 10) { x = x + 1; }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Check that we have one statement
    assert(statements.size() == 1);
    
    std::cout << "While loop parsing test passed!" << std::endl;
}

int main() {
    try {
        testWhileLoopParsing();
        std::cout << "All while loop tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}