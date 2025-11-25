#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/semantic_analyzer.h"
#include "../include/code_generator.h"
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
    
    std::cout << "=== Tokens ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Line " << token.line << ", Col " << token.column 
                  << ": " << token.lexeme << " (" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    // Parse
    try {
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        std::cout << "\n=== Parse Successful ===" << std::endl;
        std::cout << "Parsed " << statements.size() << " statements" << std::endl;
        
        // Semantic Analysis
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        std::cout << "\n=== Semantic Analysis Successful ===" << std::endl;
        
        // Code Generation
        CodeGenerator generator;
        std::string generatedCode = generator.generate(statements);
        
        std::cout << "\n=== Generated Code ===" << std::endl;
        std::cout << generatedCode << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}