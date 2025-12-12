#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/semantic_analyzer.h"
#include "../include/code_generator.h"
#include "../include/interpreter.h"
#include "../include/js_transpiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#define SYNTHFLOW_VERSION "0.0.25"

void printVersion() {
    std::cout << "SynthFlow Programming Language v" << SYNTHFLOW_VERSION << std::endl;
    std::cout << "Copyright (c) 2024 WEE Technology Solution" << std::endl;
}

void printHelp(const char* programName) {
    std::cout << "SynthFlow Programming Language" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << programName << " run <file.sf>           Execute a SynthFlow program" << std::endl;
    std::cout << "  " << programName << " compile <file.sf>       Compile and show analysis" << std::endl;
    std::cout << "  " << programName << " transpile <file.sf>     Convert to JavaScript" << std::endl;
    std::cout << "  " << programName << " <file.sf>               Same as 'run'" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v, --version    Show version information" << std::endl;
    std::cout << "  -h, --help       Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " run hello.sf" << std::endl;
    std::cout << "  " << programName << " transpile app.sf > app.js" << std::endl;
    std::cout << "  " << programName << " --version" << std::endl;
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int runProgram(const std::string& source) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        Interpreter interpreter;
        interpreter.execute(statements);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int compileProgram(const std::string& source) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Line " << token.line << ", Col " << token.column 
                      << ": " << token.lexeme << " (" << static_cast<int>(token.type) << ")" << std::endl;
        }
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        std::cout << "\n=== Parse Successful ===" << std::endl;
        std::cout << "Parsed " << statements.size() << " statements" << std::endl;
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        std::cout << "\n=== Semantic Analysis Successful ===" << std::endl;
        
        CodeGenerator generator;
        std::string generatedCode = generator.generate(statements);
        
        std::cout << "\n=== Generated Code ===" << std::endl;
        std::cout << generatedCode << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int transpileProgram(const std::string& source) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        JSTranspiler transpiler;
        std::string jsCode = transpiler.transpile(statements);
        
        // Output to stdout (can be redirected to file)
        std::cout << jsCode;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    // Check for flags
    if (argc >= 2) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printVersion();
            return 0;
        }
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printHelp(argv[0]);
            return 0;
        }
    }
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [run|compile|transpile] <source_file>" << std::endl;
        std::cerr << "Try '" << argv[0] << " --help' for more information." << std::endl;
        return 1;
    }
    
    std::string command;
    std::string filePath;
    
    // Determine command and file
    if (argc >= 3 && (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "compile") == 0 || strcmp(argv[1], "transpile") == 0)) {
        command = argv[1];
        filePath = argv[2];
    } else {
        // Default to run if just a file is provided
        command = "run";
        filePath = argv[1];
    }
    
    // Read source file
    std::string source;
    try {
        source = readFile(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Execute command
    if (command == "run") {
        return runProgram(source);
    } else if (command == "compile") {
        return compileProgram(source);
    } else if (command == "transpile") {
        return transpileProgram(source);
    }
    
    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
}

