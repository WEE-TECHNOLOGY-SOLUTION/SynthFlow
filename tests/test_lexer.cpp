#include "lexer.h"
#include <iostream>
#include <cassert>

int main() {
    // Test basic tokenization
    std::string source = "fn add(x: Int, y: Int) -> Int {\n    return x + y;\n}";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // Print tokens for verification
    for (const auto& token : tokens) {
        std::cout << "Type: " << static_cast<int>(token.type) 
                  << ", Lexeme: '" << token.lexeme 
                  << "', Line: " << token.line 
                  << ", Column: " << token.column << std::endl;
    }
    
    // Basic assertions
    assert(tokens.size() > 0);
    assert(tokens[0].type == TokenType::KW_FN);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "add");
    
    std::cout << "Lexer test passed!" << std::endl;
    return 0;
}