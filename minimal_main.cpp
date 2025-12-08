#include "compiler/include/lexer.h"
#include "compiler/include/token.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // When double-clicked, show helpful message and try to use default example
        std::cout << "========================================" << std::endl;
        std::cout << "  SynthFlow Lexer - Interactive Mode" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Usage: Drag and drop a .synth file onto this executable" << std::endl;
        std::cout << "Or run from command line: synthflow.exe <source_file>" << std::endl;
        std::cout << std::endl;
        std::cout << "Trying to read default example file..." << std::endl;
        
        // Try to read the example file that should be in the same directory
        std::ifstream file("examples/hello.synth");
        if (!file.is_open()) {
            std::cout << "Could not find examples/hello.synth" << std::endl;
            std::cout << "Please provide a .synth file as an argument." << std::endl;
            std::cout << std::endl;
            std::cout << "Press Enter to exit...";
            std::cin.get();
            return 1;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        file.close();
        
        std::cout << "Analyzing examples/hello.synth..." << std::endl;
        std::cout << std::endl;
        
        // Tokenize
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Line " << token.line << ", Col " << token.column 
                      << ": " << token.lexeme << " (" << static_cast<int>(token.type) << ")" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Analysis complete!" << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 0;
    }
    
    // Read source file from command line argument
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
    
    std::cout << "=== Tokens ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Line " << token.line << ", Col " << token.column 
                  << ": " << token.lexeme << " (" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    return 0;
}