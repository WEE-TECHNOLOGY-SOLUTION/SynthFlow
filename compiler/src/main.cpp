#include "lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }
    
    // Read source file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    // Tokenize
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // Output tokens
    for (const auto& token : tokens) {
        std::cout << "Line " << token.line << ", Col " << token.column 
                  << ": " << token.lexeme << " (" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    return 0;
}