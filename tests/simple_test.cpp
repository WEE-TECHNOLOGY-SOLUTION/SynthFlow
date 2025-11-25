#include "compiler/include/lexer.h"
#include <iostream>
#include <string>

int main() {
    std::string source = "let x = 42;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Type: " << static_cast<int>(token.type) 
                  << ", Lexeme: '" << token.lexeme 
                  << "', Line: " << token.line 
                  << ", Column: " << token.column << std::endl;
    }
    
    return 0;
}